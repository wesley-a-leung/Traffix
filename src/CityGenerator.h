#ifndef CITYGENERATOR_H
#define CITYGENERATOR_H

#include <cmath>
#include "framework/Framework.h"

#define MAX_DEPTH 8
#define MIN_PARTITION 0.3
#define MAX_PARTITION 0.7
#define MIN_CAPACITY 200
#define MAX_CAPACITY 2000
#define MIN_SPEED_LIMIT 30
#define MAX_SPEED_LIMIT 100

struct CityGenerator {
private:
    int seed;
    WeightedDigraph *G;
    void generate(std::pair<RoadSegment*, RoadSegment*> top, std::pair<RoadSegment*, RoadSegment*> bottom,
        std::pair<RoadSegment*, RoadSegment*> left, std::pair<RoadSegment*, RoadSegment*> right, int depth);

public:
    CityGenerator(Point2D topLeft, Point2D topRight, Point2D bottomLeft, Point2D bottomRight, int seed);
    ~CityGenerator();
    WeightedDigraph *getGraph() const;
};

#endif
