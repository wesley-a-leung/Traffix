#include <cstdio>
#include <chrono>
#include <cmath>
#include <assert.h>
#include "ConsoleDriver.h"
#include "controller/PretimedController.h"
#include "controller/BasicController.h"

using namespace std;

/**
 * Initializes a new ConsoleDriver.
 * @param iterationsPerSecond the number of iterations to be executed in the simulator per second
 * @param file the file to load the city
 * @param controllerType 0 if PretimedController, 1 for BasicController
 */
ConsoleDriver::ConsoleDriver(double iterationsPerSecond, string file, int controllerType) {
    assert(iterationsPerSecond > 0.0 && "iterationsPerSecond must be a positive value");
    this->iterationsPerSecond = iterationsPerSecond;
    iterationLength = 1.0 / iterationsPerSecond;
    freopen(file.c_str(), "r", stdin);
    G = new WeightedDigraph();
    if (controllerType == 0) controller = new PretimedController(G);
    else if (controllerType == 1) controller = new BasicController(G);
    sim = new Simulation(controller);
    int cntIntersections;
    int cntRoadSegments;
    int cntCars;
    scanf("%d %d %d %d", &cntIntersections, &cntRoadSegments, &cntCars, &carsPerSecond);
    Intersection *intersections[cntIntersections];
    for (int i = 0; i < cntIntersections; i++) {
        double x;
        double y;
        scanf("%lf %lf", &x, &y);
        intersections[i] = new Intersection(x, y);
    }
    for (int i = 0; i < cntRoadSegments; i++) {
        int A;
        int B;
        double speedLimit;
        int capacity;
        scanf("%d %d %lf %d", &A, &B, &speedLimit, &capacity);
        assert(G->addRoadSegment(new RoadSegment(intersections[A], intersections[B], speedLimit, capacity)));
    }
    for (int i = 0; i < cntIntersections; i++) {
        intersections[i]->autoConnectAndLink();
    }
    for (int i = 0; i < cntCars; i++) {
        Car *c = getRandomCar(G, 0.0);
        c->setSpeed(c->getCurrentRoad()->getSpeedLimit());
    }
    for (int i = 0; i < cntIntersections; i++) {
        controller->addEvent(0.0, intersections[i]->getID());
    }
}

/**
 * Deconstructs the ConsoleDriver and the associated simulation.
 */
ConsoleDriver::~ConsoleDriver() {
    delete sim;
}

/**
 * Runs the simulation in the console.
 */
void ConsoleDriver::run() {
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
                c->setSpeed(c->getCurrentRoad()->getSpeedLimit());
            }
            lastCarSpawn = end;
        }
        clearConsole();
        printToConsole();
    }
}


/**
 * Clears the console
 */
void ConsoleDriver::clearConsole() {
    for (int i = 0; i < 200; i++) printf("\n");
}

/**
 * Prints a grid city layout to the console.
 */
void ConsoleDriver::printToConsole() {
    char grid[CONSOLE_GRID_SIZE + 5][CONSOLE_GRID_SIZE + 5]; // characer grid on console
    for (int i = 0; i < CONSOLE_GRID_SIZE; i++) {
        for (int j = 0; j < CONSOLE_GRID_SIZE; j++) {
            grid[i][j] = ' ';
        }
    }
    for (pair<int, Intersection*> i : G->getIntersections()) {
        Point2D loc = i.second->getLocation();
        int x = (int) (loc.x * SCALE_FACTOR);
        int y = (int) (loc.y * SCALE_FACTOR);
        grid[y][x] = '+';
        grid[y][x + 1] = '+';
        grid[y + 1][x] = '+';
        grid[y + 1][x + 1] = '+';
    }
    for (pair<int, RoadSegment*> r : G->getRoadSegments()) {
        Point2D src = r.second->getSource()->getLocation(), dest = r.second->getDestination()->getLocation();
        int srcX = (int) (src.x * SCALE_FACTOR), srcY = (int) (src.y * SCALE_FACTOR), destX = (int) (dest.x * SCALE_FACTOR), destY = (int) (dest.y * SCALE_FACTOR);
        if (srcX == destX) {
            int adj = srcY > destY ? 1 : 0;
            for (int y = min(srcY, destY); y <= max(srcY, destY); y++) {
                if (grid[y][srcX + adj] == ' ') grid[y][srcX + adj] = '|';
            }
            grid[(srcY + destY) / 2][srcX + adj] = '0' + (int) ceil(r.second->getFlow() * 5 / r.second->getCapacity());
        } else if (srcY == destY) {
            int adj = srcX < destX ? 1 : 0;
            for (int x = min(srcX, destX); x <= max(srcX, destX); x++) {
                if (grid[srcY + adj][x] == ' ') grid[srcY + adj][x] = '-';
            }
            grid[srcY + adj][(srcX + destX) / 2] = '0' + (int) ceil(r.second->getFlow() * 5 / r.second->getCapacity());
        }
    }
    for (int i = 0; i < CONSOLE_GRID_SIZE + 2; i++) {
        printf("-");
    }
    printf("\n");
    for (int i = 0; i < CONSOLE_GRID_SIZE; i++) {
        printf("|") ;
        for (int j = 0; j < CONSOLE_GRID_SIZE; j++) {
            printf("%c", grid[i][j]);
        }
        printf("|\n");
    }
    for (int i = 0; i < CONSOLE_GRID_SIZE + 2; i++) {
        printf("-");
    }
    printf("\n");
}
