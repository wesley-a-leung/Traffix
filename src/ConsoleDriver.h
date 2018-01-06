#ifndef CONSOLEDRIVER_H_
#define CONSOLEDRIVER_H_

#include <string>
#include "controller/Controller.h"
#include "Simulation.h"
#include "framework/Framework.h"

#define CONSOLE_GRID_SIZE 50
#define SCALE_FACTOR 0.1

/**
 * The driver behind the console display.
 */
struct ConsoleDriver {
private:
    Controller *controller; // the traffic controller
    Simulation *sim; // the simulation being run
    WeightedDigraph *G; // the city represented as a weighted directed graph
    double iterationsPerSecond; // the number of iterations per second the simulation should execute
    double iterationLength; // the length of one iteration
    int carsPerSecond; // the number of cars added per iteration

    void clearConsole();
    void printToConsole();

public:
    ConsoleDriver(double iterationsPerSecond, std::string file, int controllerType);
    ~ConsoleDriver();
    void run();
};

#endif
