#ifndef SIMULATION_H_
#define SIMULATION_H_

#include "controller/Controller.h"
#include "framework/Framework.h"

#define REACTION_TIME 0.1

/**
 * Simulates the traffic in the city
 */
struct Simulation {
private:
    Controller *controller; // the traffic controller
    double currentTime; // the time elapsed in the simulation

public:
    Simulation(Controller *controller);
    ~Simulation();
    double getCurrentTime();
    void nextIteration(double timeElapsed);
};

#endif
