#include <assert.h>
#include "BasicController.h"

using namespace std;

/**
 * Initializes the BasicController given a Weighted Directed Graph.
 * @param G the Weighted Directed Graph that the controller will control
 */
BasicController::BasicController(WeightedDigraph *G) : Controller(G) {}

/**
 * Deconstructs the BasicController.
 */
BasicController::~BasicController() {}

/**
 * Adds an event with a specified intersection ID at a specified time.
 * The event is when the traffic signal is scheduled to be cycled.
 * @param time the specified time the event will occur
 * @param id the intersection ID of the event
 */
void BasicController::addEvent(double time, int id) {
    G->getIntersection(id)->setScheduledTime(time);
    events.push(make_pair(time, id));
}

/**
 * Returns true if the current time is at or after the time the next event is scheduled to occur.
 * Returns false otherwise.
 * @param currentTime the current time
 */
bool BasicController::checkNextEvent(double currentTime) const {
    return currentTime >= events.top().first;
}

/**
 * Runs the events that are before or at the current time that have not yet been run and adds the
 * next events to the event queue.
 * @param currentTime the current time
 */
void BasicController::runEvents(double currentTime) {
    while (!events.empty() && events.top().first <= currentTime) {
        Intersection *n = G->getIntersection(events.top().second);
        events.pop();
        n->cycle(currentTime);
        assert(!n->isScheduled());
        if (n->leftTurnSignalOn()) {
            addEvent(currentTime + LEFT_SIGNAL_TIME, n->getID());
            assert(n->isScheduled());
        }
    }
    // get current cycle number in intersection, if green, check if net flow is less than 2 times the opposite flow
    // if so, then cycle the lights
    for (pair<int, Intersection*> n : G->getIntersections()) {
        if (currentTime - n.second->getTimeOfLastCycle() < MIN_TIME || n.second->leftTurnSignalOn() || n.second->getScheduledTime() > currentTime) {
            continue;
        } else if (currentTime - n.second->getTimeOfLastCycle() >= MAX_TIME && n.second->getOppositeFlow() != 0) {
            addEvent(currentTime + COOLDOWN, n.first);
            assert(n.second->isScheduled());
        } else if (n.second->getCurrentFlow() < 2 * n.second->getOppositeFlow()) {
            addEvent(currentTime + COOLDOWN, n.first);
            assert(n.second->isScheduled());
        }
    }
}
