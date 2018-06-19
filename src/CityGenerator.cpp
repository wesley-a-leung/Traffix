#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <utility>
#include "CityGenerator.h"

using namespace std;

mt19937 CityGenerator::generator;

/**
 * Constructs a CityGenerator object.
 */
CityGenerator::CityGenerator(Point2D topLeft, Point2D topRight, Point2D bottomLeft, Point2D bottomRight, int seed) : distribution(MIN_PARTITION, MAX_PARTITION) {
    this->seed = seed;
    generator.seed(seed);
    G = new WeightedDigraph();
    Intersection *topLeftIntxn = new Intersection(topLeft);
    Intersection *topRightIntxn = new Intersection(topRight);
    Intersection *bottomLeftIntxn = new Intersection(bottomLeft);
    Intersection *bottomRightIntxn = new Intersection(bottomRight);
    RoadSegment *tltr = new RoadSegment(topLeftIntxn, topRightIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(tltr);
    RoadSegment *trtl = new RoadSegment(topRightIntxn, topLeftIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(trtl);
    RoadSegment *blbr = new RoadSegment(bottomLeftIntxn, bottomRightIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(blbr);
    RoadSegment *brbl = new RoadSegment(bottomRightIntxn, bottomLeftIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(brbl);
    RoadSegment *tlbl = new RoadSegment(topLeftIntxn, bottomLeftIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(tlbl);
    RoadSegment *bltl = new RoadSegment(bottomLeftIntxn, topLeftIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(bltl);
    RoadSegment *trbr = new RoadSegment(topRightIntxn, bottomRightIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(trbr);
    RoadSegment *brtr = new RoadSegment(bottomRightIntxn, topRightIntxn, MAX_SPEED_LIMIT, MAX_CAPACITY);
    G->addRoadSegment(brtr);
    pair<RoadSegment*, RoadSegment*> top = make_pair(tltr, trtl);
    pair<RoadSegment*, RoadSegment*> bottom = make_pair(blbr, brbl);
    pair<RoadSegment*, RoadSegment*> left = make_pair(tlbl, bltl);
    pair<RoadSegment*, RoadSegment*> right = make_pair(trbr, brtr);
    generate(top, bottom, left, right, 0);
}

/**
 * Deconstructs a CityGenerator object.
 */
CityGenerator::~CityGenerator() { delete G; }

/**
 * Return the WeightedDigraph of the generated city.
 */
WeightedDigraph *CityGenerator::getGraph() const { return G; };

/**
 * Recursive function to generate the city.
 */
void CityGenerator::generate(pair<RoadSegment*, RoadSegment*> top, pair<RoadSegment*, RoadSegment*> bottom,
        pair<RoadSegment*, RoadSegment*> left, pair<RoadSegment*, RoadSegment*> right, int depth) {
    if (depth == MAX_DEPTH) return;
    // srand(seed * top.first->getID() * left.first->getID()); // NOTE i left this here cause idk why Wesley has this here
    double factor1 = distribution(generator);
    double factor2 = distribution(generator);
    double speed = (MAX_SPEED_LIMIT - MIN_SPEED_LIMIT) / MAX_DEPTH * (MAX_DEPTH - depth) + MIN_SPEED_LIMIT;
    double capacity = (MAX_CAPACITY - MIN_CAPACITY) / MAX_DEPTH * (MAX_DEPTH - depth) + MIN_CAPACITY;
    Point2D topLeft = top.first->getSource()->getLocation();
    Point2D topRight = top.second->getSource()->getLocation();
    Point2D bottomLeft = bottom.first->getSource()->getLocation();
    Point2D bottomRight = bottom.second->getSource()->getLocation();
    if (depth % 2 == 0) {
        double dxTop = topRight.x - topLeft.x;
        double dyTop = topRight.y - topLeft.y;
        double dxBottom = bottomRight.x - bottomLeft.x;
        double dyBottom = bottomRight.y - bottomLeft.y;
        Point2D newTop(topLeft.x + dxTop * factor1, topLeft.y + dyTop * factor1);
        Point2D newBottom(bottomLeft.x + dxBottom * factor2, bottomLeft.y + dyBottom * factor2);
        Intersection *topIntxn = new Intersection(newTop);
        Intersection *bottomIntxn = new Intersection(newBottom);
        pair<RoadSegment*, RoadSegment*> topLeftward = G->splitRoadSegment(top.first, topIntxn);
        pair<RoadSegment*, RoadSegment*> topRightward = G->splitRoadSegment(top.second, topIntxn);
        pair<RoadSegment*, RoadSegment*> bottomLeftward = G->splitRoadSegment(bottom.first, bottomIntxn);
        pair<RoadSegment*, RoadSegment*> bottomRightward = G->splitRoadSegment(bottom.second, bottomIntxn);
        RoadSegment *downwardRoad = new RoadSegment(topIntxn, bottomIntxn, speed, capacity);
        RoadSegment *upwardRoad = new RoadSegment(bottomIntxn, topIntxn, speed, capacity);
        G->addRoadSegment(downwardRoad);
        G->addRoadSegment(upwardRoad);
        if (depth % 4) generate(make_pair(topLeftward.first, topRightward.second), make_pair(bottomLeftward.first, bottomRightward.second),
            left, make_pair(downwardRoad, upwardRoad), depth + 1);
        else generate(make_pair(topLeftward.second, topRightward.first), make_pair(bottomLeftward.second, bottomRightward.first),
            make_pair(downwardRoad, upwardRoad), right, depth + 1);
    } else {
        double dxLeft = bottomLeft.x - topLeft.x;
        double dyLeft = bottomLeft.y - topLeft.y;
        double dxRight = bottomRight.x - topRight.x;
        double dyRight = bottomRight.y - topRight.y;
        Point2D newLeft(topLeft.x + dxLeft * factor1, topLeft.y + dyLeft * factor1);
        Point2D newRight(topRight.x + dxRight * factor2, topRight.y + dyRight * factor2);
        Intersection *leftIntxn = new Intersection(newLeft);
        Intersection *rightIntxn = new Intersection(newRight);
        pair<RoadSegment*, RoadSegment*> leftDownward = G->splitRoadSegment(left.first, leftIntxn);
        pair<RoadSegment*, RoadSegment*> leftUpward = G->splitRoadSegment(left.second, leftIntxn);
        pair<RoadSegment*, RoadSegment*> rightDownward = G->splitRoadSegment(right.first, rightIntxn);
        pair<RoadSegment*, RoadSegment*> rightUpward = G->splitRoadSegment(right.second, rightIntxn);
        RoadSegment *leftwardRoad = new RoadSegment(leftIntxn, rightIntxn, speed, capacity);
        RoadSegment *rightwardRoad = new RoadSegment(rightIntxn, leftIntxn, speed, capacity);
        G->addRoadSegment(leftwardRoad);
        G->addRoadSegment(rightwardRoad);
        if ((depth - 1) % 4)generate(top, make_pair(leftwardRoad, rightwardRoad),
            make_pair(leftDownward.first, leftUpward.second), make_pair(rightDownward.first, rightUpward.second), depth + 1);
        else generate(make_pair(leftwardRoad, rightwardRoad), bottom,
            make_pair(leftDownward.second, leftUpward.first), make_pair(rightDownward.second, rightUpward.first), depth + 1);
    }
}
