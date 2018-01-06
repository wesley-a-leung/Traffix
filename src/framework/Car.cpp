#include <cstdlib>
#include <cmath>
#include <ctime>
#include <limits>
#include <assert.h>
#include "Car.h"

using namespace std;

int Car::counter = 0; // counter starts at 0
double Car::efficiency = 1.0;
int Car::reached = 0;

/**
 * Initializes a car given the starting and ending point.
 * @param source the exact location of the source in the x, y plane
 * @param destination the exact location of the destination in the x, y plane
 * @param sourceRoads the road segments the lead out of the source
 * @param destinationRoads the road segments that lead into the destination
 * @param currentTime the current time in the simulation
 * @param G the Weighted Directed Graph
 */
Car::Car(Point2D &source, Point2D &destination, vector<RoadSegment*> &sourceRoads, vector<RoadSegment*> &destinationRoads, double currentTime, WeightedDigraph *G) {
    this->source = source;
    this->destination = destination;
    this->sourceRoads = sourceRoads;
    this->destinationRoads = destinationRoads;
    this->expectedTime = 0.0;
    id = counter++;
    for (RoadSegment *r : sourceRoads) {
        assert(r->getCapacity() - r->getFlow() >= 1);
        sourceIntersections.push_back(r->getDestination()->getID());
        initialTime.push_back(r->getDestination()->getLocation().distanceTo(this->source) / r->getLength() * r->getExpectedTime());
    }
    for (RoadSegment *r : destinationRoads) {
        destinationIntersections.push_back(r->getSource()->getID());
        excessTime.push_back(r->getSource()->getLocation().distanceTo(this->destination) / r->getLength() * r->getExpectedTime());
    }
    path = new DijkstraDirectedSP(G, sourceIntersections, initialTime, destinationIntersections, excessTime);
    assert(path->hasPath() && "there is no path for the car to reach the destination from the source");
    for (RoadSegment *r : path->getShortestPath()) {
        expectedTime += r->getExpectedTime();
    }
    currentLocation = this->source;
    for (RoadSegment *r : sourceRoads) {
        if (r->getDestination()->getID() == path->getSourceID()) {
            currentRoad = r;
            expectedTime += currentRoad->getDestination()->getLocation().distanceTo(source) / currentRoad->getSpeedLimit();
            break;
        }
    }
    assert(currentRoad != nullptr);
    for (RoadSegment *r : destinationRoads) {
        if (r->getSource()->getID() == path->getDestinationID()) {
            finalRoad = r;
            expectedTime += finalRoad->getSource()->getLocation().distanceTo(destination) / finalRoad->getSpeedLimit();
            break;
        }
    }
    assert(finalRoad != nullptr);
    pathIndex = -1;
    currentRoad->addIncoming(this);
    assert(currentRoad->addCar(this));
    this->startTime = currentTime;
}

/**
 * Returns the unique ID of the car.
 */
int Car::getID() const { return id; }

/**
 * Returns the time elapsed in the car's jouney so far.
 */
double Car::getElapsedTime(double currentTime) const { return currentTime - startTime; }

/**
 * Returns the time expected for the car to complete its journey assuming no traffic.
 */
double Car::getExpectedTime() const { return expectedTime; }

/**
 * Returns the car's current speed.
 */
double Car::getCurrentSpeed() const { return currentSpeed; }

/**
 * Sets the car's speed.
 */
void Car::setSpeed(double speed) { currentSpeed = speed; }

/**
 * Returns the road the car is currently on.
 */
RoadSegment *Car::getCurrentRoad() const { return currentRoad; }

/**
 * Returns the final road the car will travel on.
 */
RoadSegment *Car::getFinalRoad() const { return finalRoad; }

/**
 * Returns true if the car has another road on its path, false otherwise.
 */
bool Car::hasNextRoad() const {
    return pathIndex + 1 <= path->getShortestPath().size();
}

/**
 * Returns the next road on the car's path if there is a next road and updates the pathIndex.
 */
RoadSegment *Car::getNextRoad() {
    assert(hasNextRoad() && "car does not have another road on its path");
    RoadSegment *r = peekNextRoad();
    pathIndex++;
    return r;
}

/**
 * Returns the next road on the car's path.
 */
RoadSegment *Car::peekNextRoad() const {
    assert(hasNextRoad() && "car does not have another road on its path");
    return pathIndex + 1 < path->getShortestPath().size() ? path->getShortestPath()[pathIndex + 1] : finalRoad;
}

/**
 * Sets the road the car is on.
 */
void Car::setRoad(RoadSegment *road) { currentRoad = road; }

/**
 * Returns the car's current location.
 */
Point2D Car::getCurrentLocation() const { return currentLocation; }

/**
 * Sets the car's location.
 */
void Car::setLocation(Point2D &location) { currentLocation = location; }

/**
 * Returns the car's source location.
 */
Point2D Car::getSource() const { return source; }

/**
 * Returns the car's destination location.
 */
Point2D Car::getDestination() const { return destination; }

/**
 * Deconstructs the Car and updates the efficiency
 */
Car::~Car() {
    delete path;
}

/**
 * Updates the efficiency of the car.
 * @param endTime the time the car reached the destination.
 */
void Car::updateEfficiency(double endTime) {
    assert(!hasNextRoad() && "car has not reached its destination");
    efficiency = ((efficiency * reached) + (expectedTime / (endTime - startTime))) / (reached + 1);
    reached++;
}

/**
 * Returns the efficiency of all cars.
 */
double Car::getEfficiency() { return efficiency; }

/**
 * Returns a random road segment in the graph.
 */
RoadSegment *getRandomRoadSegment(WeightedDigraph *G) {
    while (true) {
        int randIndex = rand() % G->countRoadSegments();
        RoadSegment *r = G->getRoadSegment(G->getRoadSegmentID(randIndex));
        if (r->getCapacity() - r->getFlow() >= 1) return r;
    }
}

/**
 * Returns a random point on the road segment.
 */
Point2D getRandomLocation(RoadSegment *r) {
    double randDist = r->getLength() * rand() / RAND_MAX;
    Point2D srcLoc = r->getSource()->getLocation(), destLoc = r->getDestination()->getLocation();
    double angle = srcLoc.angleTo(destLoc);
    double dx = randDist * cos(angle);
    double dy = randDist * sin(angle);
    return Point2D(srcLoc.x + dx, srcLoc.y + dy);
}

/**
 * Returns a car with a randomly generated source and destination.
 * MAKE SURE THAT RAND HAS A SEED
 */
Car *getRandomCar(WeightedDigraph *G, double currentTime) {
    RoadSegment *src = getRandomRoadSegment(G);
    RoadSegment *dest = nullptr;
    do {
        dest = getRandomRoadSegment(G);
    } while (src->getID() == dest->getID() || src->getDestination()->getID() == dest->getSource()->getID() || src->getSource()->getID() == dest->getDestination()->getID());
    vector<RoadSegment*> sourceRoads, destinationRoads;
    sourceRoads.push_back(src);
    destinationRoads.push_back(dest);
    Point2D srcLoc = getRandomLocation(src);
    Point2D destLoc = getRandomLocation(dest);
    return new Car(srcLoc, destLoc, sourceRoads, destinationRoads, currentTime, G);
}
