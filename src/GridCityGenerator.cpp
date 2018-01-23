#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <utility>
#include "GridCityGenerator.h"

using namespace std;

/**
 * Constructs a GridCityGenerator object and generates the grid city.
 */
GridCityGenerator::GridCityGenerator(Point2D topLeft, Point2D topRight, Point2D bottomLeft, Point2D bottomRight, int seed) {
    this->seed = seed;
    srand(seed);
    G = new WeightedDigraph();
    double top = topLeft.y;
    double right = topRight.x;
    double bottom = bottomRight.y;
    double left = bottomLeft.x;
    double xInterval = (right - left) / INTERVALS;
    double yInterval = (bottom - top) / INTERVALS;
    Intersection ***grid = new Intersection**[INTERVALS + 1];
    for (int i = 0; i <= INTERVALS; i++) {
        grid[i] = new Intersection*[INTERVALS + 1];
        for (int j = 0; j <= INTERVALS; j++) {
            Point2D p(top + i * xInterval, left + j * yInterval);
            grid[i][j] = new Intersection(p);
        }
    }
    for (int i = 0; i <= INTERVALS; i++) {
        for (int j = 0; j <= INTERVALS; j++) {
            if (i > 0) G->addRoadSegment(new RoadSegment(grid[i][j], grid[i - 1][j], SPEED_LIMIT, CAPACITY));
            if (i < INTERVALS) G->addRoadSegment(new RoadSegment(grid[i][j], grid[i + 1][j], SPEED_LIMIT, CAPACITY));
            if (j > 0) G->addRoadSegment(new RoadSegment(grid[i][j], grid[i][j - 1], SPEED_LIMIT, CAPACITY));
            if (j < INTERVALS) G->addRoadSegment(new RoadSegment(grid[i][j], grid[i][j + 1], SPEED_LIMIT, CAPACITY));
        }
    }
}

/**
 * Deconstructs a CityGenerator object.
 */
GridCityGenerator::~GridCityGenerator() { delete G; }

/**
 * Return the WeightedDigraph of the generated city.
 */
WeightedDigraph *GridCityGenerator::getGraph() const { return G; };
