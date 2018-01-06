#include "Controller.h"

/**
 * Initializes the Controller given a Weighted Directed Graph.
 * @param G the Weighted Directed Graph that the controller will control
 */
Controller::Controller(WeightedDigraph *G) {
    this->G = G;
}

/**
 * Deconstructs the Controller.
 */
Controller::~Controller() {}

/**
 * Returns a pointer to the weighted directed graph.
 */
WeightedDigraph *Controller::getGraph() const { return G; }
