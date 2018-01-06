#include <utility>
#include <cmath>
#include <unordered_set>
#include <assert.h>
#include "Simulation.h"
#include "controller/PretimedController.h"
#include "misc/pair_hash.h"

using namespace std;

/**
 * Initializes a new Simulation given a controller.
 * @param controller the controller that will handle the traffic
 */
Simulation::Simulation(Controller *controller) {
    this->controller = controller;
    currentTime = 0.0;
}

/**
 * Deconstructs the Simulation.
 */
Simulation::~Simulation() {}

/**
 * Returns the current time in the simulation.
 */
double Simulation::getCurrentTime() { return currentTime; }

/**
 * Performs the next iteration in the simulation.
 * @param timeElapsed the time elasped since the last iteration
 */
void Simulation::nextIteration(double timeElapsed) {
    currentTime += timeElapsed;
    controller->runEvents(currentTime);
    WeightedDigraph *G = controller->getGraph();
    unordered_set<int> done;
    unordered_set<pair<int, int>, pair_hash<int, int>> toDelete;
    // PRE CHECK
    // for (pair<int, RoadSegment*> r : G->getRoadSegments()) {
    //     for (pair<int, Car*> c : r.second->getCars()) {
    //         assert(c.second != nullptr);
    //         if (c.second->hasNextRoad()) {
    //             // assert(c.second->peekNextRoad()->incoming.count(c.first));
    //             assert(r.second->getDestination()->isConnected(r.first, c.second->peekNextRoad()->getID()));
    //         }
    //     }
    // }
    for (pair<int, RoadSegment*> r : G->getRoadSegments()) {
        Point2D dest = r.second->getDestination()->getLocation();
        // HANDLES CARS WAITING IN THE QUEUE TO EXIT INTERSECTION
        if (r.second->countCarsInQueue() > 0 && r.second->getLatestTime() + REACTION_TIME <= currentTime) {
            Car *c = r.second->getNextCarFromQueue();
            done.insert(c->getID());
            if (!c->hasNextRoad() || (r.second->getDestination()->getLightBetween(r.first, c->peekNextRoad()->getID())->getState() == GREEN
                    && c->peekNextRoad()->getCapacity() - c->peekNextRoad()->getFlow() >= 1)) {
                r.second->removeNextCarFromQueue(currentTime);
                c->setLocation(dest);
                assert(r.second->removeCar(c) && "car not on road");
                if (c->hasNextRoad()) {
                    RoadSegment *rp = c->getNextRoad();
                    assert(rp->addCar(c) && "car was already on road");
                    c->setLocation(dest);
                } else { // car has reached destination
                    c->updateEfficiency(currentTime);
                    delete c;
                }
            }
        }
        toDelete.clear();
        // HANDLES CARS TRAVELLING AT ROAD SPEED
        for (pair<int, Car*> c : r.second->getCars()) {
            if (done.count(c.first) == 0 && !r.second->isStopped(c.first)) {
                done.insert(c.first);
                double dx = (timeElapsed * c.second->getCurrentSpeed()) * cos(c.second->getCurrentLocation().angleTo(dest));
                double dy = (timeElapsed * c.second->getCurrentSpeed()) * sin(c.second->getCurrentLocation().angleTo(dest));
                Point2D newLoc(c.second->getCurrentLocation().x + dx, c.second->getCurrentLocation().y + dy);
                c.second->setLocation(newLoc);
                double eps_dist = timeElapsed * c.second->getCurrentSpeed() * 0.51; // max distance between frames
                if (c.second->getDestination().distanceTo(newLoc) <= eps_dist && r.first == c.second->getFinalRoad()->getID()) {
                    toDelete.insert(make_pair(c.first, 1)); // type 1 means car has reached its destination
                } else if (r.second->countCarsInQueue() > 0 && r.second->getLastCarInQueue()->getCurrentLocation().distanceTo(newLoc) <= eps_dist
                        && (!c.second->hasNextRoad() || r.second->getDestination()->getLightBetween(r.first, c.second->peekNextRoad()->getID())->getType() == LEFT)) {
                    assert(r.second->stop(c.first)); // if there are cars stopped ahead (that are turning left), then this car should also stop
                } else if (dest.distanceTo(newLoc) <= eps_dist) {
                    if (!c.second->hasNextRoad() || (r.second->getDestination()->getLightBetween(r.first, c.second->peekNextRoad()->getID())->getState() == GREEN
                            && c.second->peekNextRoad()->getCapacity() - c.second->peekNextRoad()->getFlow() >= 1)) {
                        toDelete.insert(make_pair(c.first, 2)); // type 2 means car has reached the end of the road
                    } else {
                        assert(r.second->stop(c.first)); // car is waiting to move off the road
                    }
                }
            }
        }
        // DELETES CARS THAT HAVE REACHED THE DESTINATION
        for (pair<int, int> c : toDelete) {
            Car *car = r.second->getCar(c.first);
            if (c.second == 1) { // type 1 means car has reached its destination
                assert(r.second->removeCar(car) && "car not on road");
                car->updateEfficiency(currentTime);
                delete car;
            } else if (car->hasNextRoad()) {
                if (car->peekNextRoad()->getCapacity() - car->peekNextRoad()->getFlow() < 1) {
                    assert(r.second->stop(c.first));
                } else {
                    assert(r.second->removeCar(car) && "car not on road");
                    RoadSegment *rp = car->getNextRoad();
                    assert(rp->addCar(car) && "car was already on road");
                    car->setLocation(dest);
                }
            } else { // car has reached end the of road, and also its destination
                assert(r.second->removeCar(car) && "car not on road");
                car->updateEfficiency(currentTime);
                delete car;
            }
        }
    }
    // POST CHECK
    // for (pair<int, RoadSegment*> r : G->getRoadSegments()) {
    //     for (pair<int, Car*> c : r.second->getCars()) {
    //         assert(c.second != nullptr);
    //         if (c.second->hasNextRoad()) {
    //             // assert(c.second->peekNextRoad()->incoming.count(c.first));
    //             assert(r.second->getDestination()->isConnected(r.first, c.second->peekNextRoad()->getID()));
    //         }
    //     }
    // }
}
