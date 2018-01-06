#include <chrono>
#include <cmath>
#include <assert.h>
#include <QString>
#include <QFile>
#include <QResource>
#include <QIODevice>
#include <QTextStream>
#include "GUIDriver.h"
#include "controller/PretimedController.h"
#include "controller/BasicController.h"

#define RANDOM_CAR_SPAWNS_PER_SECOND 100
#define SEED 0x3f3f3f

const Point2D topLeft(50.0, 50.0);
const Point2D topRight(1950.0, 50.0);
const Point2D bottomLeft(50.0, 1950.0);
const Point2D bottomRight(1950.0, 1950.0);

using namespace std;

/**
 * Initializes a new GUIDriver.
 * @param argc should be the argument from main
 * @param argv should be the array of arguments from main
 * @param iterationsPerSecond the number of iterations to be executed in the simulator per second
 * @param fileName the file to load the city
 * @param controllerType 0 if PretimedController, 1 for BasicController
 * @param random true if random graph generator, false otherwise
 */
GUIDriver::GUIDriver(int argc, char *argv[], double iterationsPerSecond, string fileName, int controllerType, bool random) {
    assert(iterationsPerSecond > 0.0 && "iterationsPerSecond must be a positive value");
    this->iterationsPerSecond = iterationsPerSecond;
    iterationLength = 1.0 / iterationsPerSecond;
    if (random) {
        cg = new CityGenerator(topLeft, topRight, bottomLeft, bottomRight, SEED);
        G = cg->getGraph();
        if (controllerType == 0) controller = new PretimedController(G);
        else if (controllerType == 1) controller = new BasicController(G);
        sim = new Simulation(controller);
        carsPerSecond = RANDOM_CAR_SPAWNS_PER_SECOND;
        for (pair<int, Intersection*> intxn : G->getIntersections()) {
            intxn.second->autoConnectAndLink();
            controller->addEvent(0.0, intxn.first);
        }
    } else {
        QResource common(QString::fromStdString(fileName));
        QFile file(common.absoluteFilePath());
        assert(file.exists() && "unable to open file");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        G = new WeightedDigraph();
        if (controllerType == 0) controller = new PretimedController(G);
        else if (controllerType == 1) controller = new BasicController(G);
        sim = new Simulation(controller);
        int cntIntersections;
        int cntRoadSegments;
        int cntCars;
        in >> cntIntersections >> cntRoadSegments >> cntCars >> carsPerSecond;
        Intersection *intersections[cntIntersections];
        for (int i = 0; i < cntIntersections; i++) {
            double x;
            double y;
            in >> x >> y;
            intersections[i] = new Intersection(x, y);
        }
        for (int i = 0; i < cntRoadSegments; i++) {
            int A;
            int B;
            double speedLimit;
            int capacity;
            in >> A >> B >> speedLimit >> capacity;
            assert(G->addRoadSegment(new RoadSegment(intersections[A], intersections[B], speedLimit, capacity)));
        }
        for (int i = 0; i < cntIntersections; i++) {
            intersections[i]->autoConnectAndLink();
        }
        for (int i = 0; i < cntCars; i++) {
            Car *c = getRandomCar(G, 0.0);
            c->setSpeed(c->getCurrentRoad()->getRandomSpeed());
        }
        for (int i = 0; i < cntIntersections; i++) {
            controller->addEvent(0.0, intersections[i]->getID());
        }
        file.close();
    }
    app = new QApplication(argc, argv);
    gui = new GUI(G);
    eventLoop = new QEventLoop(gui);
    gui->show();
}

/**
 * Deconstructs the GUIDriver and the associated simulation.
 */
GUIDriver::~GUIDriver() {
    delete cg;
    delete sim;
    delete gui;
    delete eventLoop;
    delete app;
}

/**
 * Runs the simulation in the console.
 */
void GUIDriver::run() {
    bool exit = false;
    auto start = chrono::high_resolution_clock::now();
    auto lastCarSpawn = chrono::high_resolution_clock::now();
    while (!exit) {
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        while (elapsed.count() < iterationLength) { // waits for an iteration
            end = chrono::high_resolution_clock::now();
            elapsed = end - start;
        }
        start = end;
        sim->nextIteration(iterationLength);
        chrono::duration<double> timeSinceLastCar = end - lastCarSpawn;
        if (timeSinceLastCar.count() >= 1.0 / ((double) carsPerSecond)) {
            for (int i = 0; i < (int) floor(timeSinceLastCar.count() * ((double) carsPerSecond)); i++) {
                Car *c = getRandomCar(G, sim->getCurrentTime());
                c->setSpeed(c->getCurrentRoad()->getRandomSpeed());
            }
            lastCarSpawn = end;
        }
        draw();
    }
}

/**
 * Displays the city layout to the GUI.
 */
void GUIDriver::draw() {
    gui->drawComponents();
    app->processEvents(eventLoop->AllEvents);
}
