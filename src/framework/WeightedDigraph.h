#ifndef WEIGHTEDDIGRAPH_H_
#define WEIGHTEDDIGRAPH_H_

#include <vector>
#include <unordered_map>
#include "Forward.h"
#include "RoadSegment.h"
#include "Intersection.h"

struct WeightedDigraph {
private:
    int intersections; // number of intersections
    int roadSegments; // number of road segments
    std::unordered_map<int, Intersection*> idToIntersection; // maps the intersection id numbers to the intersection
    std::unordered_map<int, RoadSegment*> idToRoadSegment; // maps the road segment id numbers to the road segment
    std::vector<int> roadSegmentIDs; // compresses the road segment id numbers to a continuous indexed vector
    std::unordered_map<int, int> compressedIndex; // maps the road segment id to its compressed index

public:
    WeightedDigraph();
    ~WeightedDigraph();
    int countIntersections() const;
    int countRoadSegments() const;
    bool addRoadSegment(RoadSegment *r);
    std::pair<RoadSegment*, RoadSegment*> splitRoadSegment(RoadSegment *r, Intersection *n);
    bool removeRoadSegment(int id);
    int outdegree(int id);
    int indegree(int id);
    const std::unordered_map<int, Intersection*> &getIntersections() const;
    Intersection *getIntersection(int id);
    const std::unordered_map<int, RoadSegment*> &getRoadSegments() const;
    RoadSegment *getRoadSegment(int id);
    const std::vector<int> &getRoadSegmentIDs() const;
    int getRoadSegmentID(int index);
    const std::unordered_map<int, int> &getCompressedIndices() const;
    int getCompressedIndex(int id);
    double getEfficiency();
};

#endif
