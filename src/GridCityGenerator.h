#ifndef GRIDCITYGENERATOR_H
#define GRIDCITYGENERATOR_H

#include <cmath>
#include "framework/Framework.h"

#define INTERVALS 10
#define CAPACITY 500
#define SPEED_LIMIT 80

struct GridCityGenerator {
private:
    int seed;
    WeightedDigraph *G;

public:
    GridCityGenerator(Point2D topLeft, Point2D topRight, Point2D bottomLeft, Point2D bottomRight, int seed);
    ~GridCityGenerator();
    WeightedDigraph *getGraph() const;
};

#endif
