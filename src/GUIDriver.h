#ifndef GUIDRIVER_H_
#define GUIDRIVER_H_

#include <string>
#include <QApplication>
#include <QEventLoop>
#include "gui/gui.h"
#include "CityGenerator.h"
#include "controller/Controller.h"
#include "Simulation.h"
#include "framework/Framework.h"

/**
 * The driver behind the gui display.
 */
struct GUIDriver {
private:
    QApplication *app; // the QApplication
    QEventLoop *eventLoop; // to update the display
    GUI *gui; // the GUI object
    CityGenerator *cg; // the city generator
    Controller *controller; // the traffic controller
    Simulation *sim; // the simulation being run
    WeightedDigraph *G; // the city represented as a weighted directed graph
    double iterationsPerSecond; // the number of iterations per second the simulation should execute
    double iterationLength; // the length of one iteration
    int carsPerSecond; // the number of cars added per second
    void draw();

public:
    GUIDriver(int argc, char *argv[], double iterationsPerSecond, std::string fileName, int controllerType, bool random);
    ~GUIDriver();
    void run();
};

#endif
