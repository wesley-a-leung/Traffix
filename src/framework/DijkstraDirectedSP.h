#ifndef DIJKSTRADIRECTEDSP_H_
#define DIJKSTRADIRECTEDSP_H_

#include <vector>
#include <unordered_map>
#include "Forward.h"
#include "RoadSegment.h"
#include "Intersection.h"
#include "WeightedDigraph.h"

struct DijkstraDirectedSP {
private:
    std::unordered_map<int, double> timeTo;
    std::unordered_map<int, RoadSegment*> roadTo;
    int shortestPathSourceID;
    int shortestPathDestinationID;
    double shortestTime;
    std::vector<RoadSegment*> shortestPath;

    void dijkstra(WeightedDigraph *G, std::vector<int> &sourceIDs, std::vector<double> &initialTime);

public:
    DijkstraDirectedSP(WeightedDigraph *G, std::vector<int> &sourceIDs, std::vector<double> &initialTime, std::vector<int> &destinationIDs, std::vector<double> &excessTime);
    ~DijkstraDirectedSP();
    bool hasPath() const;
    double getShortestTime() const;
    int getSourceID() const;
    int getDestinationID() const;
    const std::vector<RoadSegment*> &getShortestPath() const;
};

#endif
