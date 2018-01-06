#include <assert.h>
#include "PretimedController.h"

using namespace std;

/**
 * Initializes the PretimedController given a Weighted Directed Graph.
 * @param G the Weighted Directed Graph that the controller will control
 */
PretimedController::PretimedController(WeightedDigraph *G) : Controller(G) {}

/**
 * Deconstructs the PretimedController.
 */
PretimedController::~PretimedController() {}

/**
 * Adds an event with a specified intersection ID at a specified time.
 * The event is when the traffic signal is scheduled to be cycled.
 * @param time the specified time the event will occur
 * @param id the intersection ID of the event
 */
void PretimedController::addEvent(double time, int id) {
    events.push(make_pair(time, id));
}

/**
 * Returns true if the current time is at or after the time the next event is scheduled to occur.
 * Returns false otherwise.
 * @param currentTime the current time
 */
bool PretimedController::checkNextEvent(double currentTime) const {
    return currentTime >= events.top().first;
}

/**
 * Runs the events that are before or at the current time that have not yet been run and adds the
 * next events to the event queue.
 * @param currentTime the current time
 */
void PretimedController::runEvents(double currentTime) {
    while (!events.empty() && events.top().first <= currentTime) {
        Intersection *n = G->getIntersection(events.top().second);
        bool prevLeft = n->leftTurnSignalOn();
        events.pop();
        n->cycle(currentTime);
        if (n->leftTurnSignalOn()) addEvent(currentTime + 10.0, n->getID()); // 10 seconds for left signals
        else addEvent(currentTime + 30.0 - (10.0 * prevLeft), n->getID()); // 20 seconds if there was just a left signal, 30 otherwise
    }
}
