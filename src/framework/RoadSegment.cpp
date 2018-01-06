#include <assert.h>
#include <cmath>
#include <ctime>
#include <random>
#include "RoadSegment.h"

using namespace std;

int RoadSegment::counter = 0; // counter starts at 0

/**
 * Initalizes the RoadSegment with values
 * @param source the source intersection
 * @param destination the destination intersection
 * @param speedLimit the speed limit of the road segment (must be a positive value)
 * @param capacity the maximum number of vehicles on the road segment (must be a non-negative integer)
 */
RoadSegment::RoadSegment(Intersection *source, Intersection *destination, double speedLimit, int capacity) {
    assert(speedLimit > 0.0 && "speed limit must be a positive value");
    assert(capacity >= 0 && "capacity must be a non-negative integer");
    this->source = source; // the intersections sould be pointers, not copies
    this->destination = destination;
    id = counter++; // assigns an id and increments the counter
    Point2D srcLoc = source->getLocation(), destLoc = destination->getLocation();
    this->length = srcLoc.distanceTo(destLoc);
    this->speedLimit = speedLimit;
    this->flow = 0;
    this->capacity = capacity;
    latestTime = 0.0;
}

/**
 * Deconstructs the Road Segment and detaches it from the intersection
 */
RoadSegment::~RoadSegment() {
    source->remove(this);
    destination->remove(this);
}

/**
 * Returns the unique ID of the road segment.
 */
int RoadSegment::getID() const { return id; }

/**
 * Returns the source intersection of the road segment.
 */
Intersection *RoadSegment::getSource() const { return source; }

/**
 * Returns the destination intersection of the road segment.
 */
Intersection *RoadSegment::getDestination() const { return destination; }

/**
 * Returns the length of the road segment.
 */
double RoadSegment::getLength() const { return length; }

/**
 * Returns the speed limit of the road segment.
 */
double RoadSegment::getSpeedLimit() const { return speedLimit; }

/**
 * Returns the expected amount of time it will take to travel on this road segment.
 */
double RoadSegment::getExpectedTime() const { return length / speedLimit; }

/**
 * Returns the current flow of the road segment.
 */
int RoadSegment::getFlow() const { return flow; }

/**
 * Returns the vehicle capacity of the road segment.
 */
int RoadSegment::getCapacity() const { return capacity; }

/**
 * Returns the projected speed of cars on this road given based on the model speedlimit * -cosh(1.25 * (flow / capacity)) + 2.
 */
double RoadSegment::getProjectedSpeed() const {
    double factor = -cosh(1.25 * (double) flow / (double) capacity) + 2.0;
    assert(factor <= 1.0 + EPS && factor >= 0.1115 - EPS);
    return factor * speedLimit;
}

/**
 * Returns a random speed of cars on this road with a mean equal to the projected speed and standard deviation of 20% of the projected speed.
 */
double RoadSegment::getRandomSpeed() const {
    default_random_engine generator(time(0));
    double proj = getProjectedSpeed();
    normal_distribution<double> distribution(proj, proj * 0.2);
    return min(max(MIN_SPEED, distribution(generator)), speedLimit);
}

/**
 * Adds the specified amount of flow to the road segment. The value added must be non-negative and the flow cannot exceed the capacity.
 * @param value the amount of flow to be added (a positve value)
 */
void RoadSegment::addFlow(int value) {
    assert(value >= 0 && "value must be non-negative");
    assert(flow + value <= capacity && "flow cannot exceed capacity");
    flow += value;
}

/**
 * Subtracts the specified amount of flow to the road segment. The value substracted must be non-negative and the flow cannot become negative.
 * @param value the amount of flow to be subtracted (a positive value)
 */
void RoadSegment::subtractFlow(int value) {
    assert(value >= 0 && "value must be non-negative");
    assert(flow - value >= 0 && "flow cannot become negative");
    flow -= value;
}

/**
 * Adds a car to the road segment.
 * Returns true if the car was added, false otherwise (car was already on the road).
 * @param c the pointer to the car
 */
bool RoadSegment::addCar(Car *c) {
    assert(incoming.count(c->getID()) > 0 && "car is not scheduled to be on this road");
    if (cars.count(c->getID()) > 0 || flow + 1 > capacity) return false;
    addFlow(1);
    cars[c->getID()] = c;
    incoming.erase(c->getID());
    c->setRoad(this);
    c->setSpeed(getRandomSpeed());
    if (c->hasNextRoad()) c->peekNextRoad()->addIncoming(c);
    return true;
}

/**
 * Removes a car from the road segment.
 * Returns true if the car was removed, false otherwise (car was not on the road).
 * @param c the pointer to the car
 */
bool RoadSegment::removeCar(Car *c) {
    assert(inQueue.count(c->getID()) == 0);
    assert(incoming.count(c->getID()) == 0);
    if (cars.count(c->getID()) == 0 || flow - 1 < 0) return false;
    subtractFlow(1);
    cars.erase(c->getID());
    c->setRoad(nullptr);
    return true;
}

/**
 * Returns true if the car is on the road, false otherwise.
 * @param c the pointer to the car
 */
bool RoadSegment::carOnRoad(Car *c) {
    return cars.count(c->getID()) > 0;
}

/**
 * Stops the car and adds it to the waiting queue. Returns true if the car was added,
 * false otherwise (car is already in the queue).
 * @param id the ID of the car
 */
bool RoadSegment::stop(int id) {
    assert(cars.count(id) > 0 && "no car with this id exists");
    if (inQueue.count(id) > 0) return false;
    waiting.push(id);
    inQueue.insert(id);
    return true;
}

/**
 * Returns the next car in the waiting queue.
 */
Car *RoadSegment::getNextCarFromQueue() {
    assert(inQueue.size() > 0 && "there are no cars in the waiting queue");
    Car *next = cars[waiting.front()];
    return next;
}

/**
 * Removes the next car in the waiting queue.
 * @param currentTime the current time in the simulation
 */
void RoadSegment::removeNextCarFromQueue(double currentTime) {
    assert(inQueue.size() > 0 && "there are no cars in the waiting queue");
    Car *next = cars[waiting.front()];
    waiting.pop();
    inQueue.erase(next->getID());
    latestTime = currentTime;
}

/**
 * Returns the last car in the waiting queue.
 */
Car *RoadSegment::getLastCarInQueue() {
    assert(inQueue.size() > 0 && "there are no cars in the waiting queue");
    Car *last = cars[waiting.back()];
    return last;
}

/**
 * Returns the number of cars in the waiting queue.
 */
int RoadSegment::countCarsInQueue() const { return inQueue.size(); };

/**
 * Returns true if the car with the specified ID is in the waiting queue, false otherwise.
 * @param id the ID of the car
 */
bool RoadSegment::isStopped(int id) const {
    assert(cars.count(id) > 0 && "car is not in road");
    return inQueue.count(id) > 0;
}

/**
 * Schedules the car to go on this road next.
 */
void RoadSegment::addIncoming(Car *c) {
    assert(incoming.count(c->getID()) == 0 && "car is already scheduled to go on this road");
    incoming.insert(c->getID());
}

/**
 * Returns the latest time a car left the waiting queue.
 */
double RoadSegment::getLatestTime() const { return latestTime; }

/**
 * Returns a pointer to a car given its ID.
 * @param id the ID of the car
 */
Car *RoadSegment::getCar(int id) {
    assert(cars.count(id) > 0 && "car is not in road");
    return cars[id];
}

/**
 * Returns an immutable reference to the cars (and their IDs) on this road segment.
 */
const unordered_map<int, Car*> &RoadSegment::getCars() const { return cars; }

/**
 * Returns the direction of this road as an angle (between -pi and pi).
 */
double RoadSegment::getDirection() const {
    Point2D sourceLoc = source->getLocation();
    Point2D destLoc = destination->getLocation();
    return sourceLoc.angleTo(destLoc);
}

/**
 * Compares this RoadSegment to that RoadSegment r by comparing their ids.
 *
 * @param  r the other road segment
 * @return true if and only if this road segment equals the other, false otherwise
 */
bool RoadSegment::operator == (const RoadSegment &r) const { return id == r.id; }

/**
 * Compares RoadSegment r to RoadSegment s by comparing their directional angle (between -pi and pi).
 * Less than comparator.
 */
bool RoadSegment::RoadSegmentPtrPolarOrderCmpLt(const RoadSegment *r, const RoadSegment *s) {
    return s->getDirection() - r->getDirection() < EPS;
}

/**
 * Compares RoadSegment r to RoadSegment s by comparing their directional angle (between -pi and pi).
 * Less than or equal to comparator.
 */
bool RoadSegment::RoadSegmentPtrPolarOrderCmpLe(const RoadSegment *r, const RoadSegment *s) {
    return s->getDirection() - r->getDirection() < -EPS;
}

/**
 * Compares RoadSegment r to RoadSegment s by comparing their directional angle (between -pi and pi).
 * Grater than comparator.
 */
bool RoadSegment::RoadSegmentPtrPolarOrderCmpGt(const RoadSegment *r, const RoadSegment *s) {
    return r->getDirection() - s->getDirection() < EPS;
}

/**
 * Compares RoadSegment r to RoadSegment s by comparing their directional angle (between -pi and pi).
 * Grater than comparator.
 */
bool RoadSegment::RoadSegmentPtrPolarOrderCmpGe(const RoadSegment *r, const RoadSegment *s) {
    return r->getDirection() - s->getDirection() < -EPS;
}
