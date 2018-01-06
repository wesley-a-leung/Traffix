#include <utility>
#include <functional>
#include <limits>
#include <stack>
#include <queue>
#include <assert.h>
#include "DijkstraDirectedSP.h"

using namespace std;

/**
 * Calculates the shortest path based on expected time to each of the possible destinations.
 * @param G the Weighted Directed Graph
 * @param sourceIDs the intersection IDs of the sources
 * @param initialTime the initial times to each of the sources
 * @param destinationIDs the intersection IDs of the possible destinations
 * @param excesstime the extra time required for each of the possible destinations
 */
DijkstraDirectedSP::DijkstraDirectedSP(WeightedDigraph *G, vector<int> &sourceIDs, vector<double> &initialTime, vector<int> &destinationIDs, vector<double> &excessTime) {
    dijkstra(G, sourceIDs, initialTime);
    shortestPathSourceID = shortestPathDestinationID = -1;
    shortestTime = numeric_limits<double>::infinity();
    for (int d = 0; d < destinationIDs.size(); d++) {
        if (timeTo[destinationIDs[d]] + excessTime[d] < shortestTime) {
            shortestTime = timeTo[destinationIDs[d]] + excessTime[d];
            shortestPathDestinationID = destinationIDs[d];
        }
    }
    if (shortestTime != numeric_limits<double>::infinity()) {
        stack<RoadSegment*> stk;
        for (RoadSegment *r = roadTo[shortestPathDestinationID]; r != nullptr; r = roadTo[r->getSource()->getID()]) {
            stk.push(r);
        }
        assert(!stk.empty() && "no path for car to reach destination from sources");
        shortestPathSourceID = stk.top()->getSource()->getID();
        while (!stk.empty()) {
            shortestPath.push_back(stk.top());
            stk.pop();
        }
    }
}

/**
 * Deconstructs the structure.
 */
DijkstraDirectedSP::~DijkstraDirectedSP() {}

/**
 * Performs Dijkstra's Single Source Shortest Path Algorithm
 * @param G the Weighted Directed Graph
 * @param sourceIDs the intersection IDs of the sources
 * @param initialTime the initial time to each of the sources
 */
void DijkstraDirectedSP::dijkstra(WeightedDigraph *G, vector<int> &sourceIDs, vector<double> &initialTime) {
    priority_queue<pair<double, int>, vector<pair<double, int>> , greater<pair<double, int>>> pq;
    for (pair<int, Intersection*> i : G->getIntersections()) {
        timeTo[i.first] = numeric_limits<double>::infinity();
    }
    for (int s = 0; s < sourceIDs.size(); s++) {
        timeTo[sourceIDs[s]] = initialTime[s];
        roadTo[sourceIDs[s]] = nullptr;
        pq.push({timeTo[sourceIDs[s]], sourceIDs[s]});
    }
    while (!pq.empty()) {
        int v = pq.top().second;
        pq.pop();
        for (pair<int, RoadSegment*> r : G->getIntersection(v)->getOutboundRoads()) {
            int w = r.second->getDestination()->getID();
            if (timeTo[w] > timeTo[v] + r.second->getExpectedTime()) {
                timeTo[w] = timeTo[v] + r.second->getExpectedTime();
                roadTo[w] = r.second;
                pq.push({timeTo[w], w});
            }
        }
    }
}

/**
 * Returns whether there is a path from the source to reach the destination.
 */
bool DijkstraDirectedSP::hasPath() const { return shortestTime != numeric_limits<double>::infinity(); }

/**
 * Returns the shortest amount of time to reach the destination.
 */
double DijkstraDirectedSP::getShortestTime() const { return shortestTime; }

/**
 * Returns the ID of starting (source) intersection of the shortest path.
 */
int DijkstraDirectedSP::getSourceID() const { return shortestPathSourceID; }

/**
 * Returns the ID of destination intersection of the shortest path.
 */
int DijkstraDirectedSP::getDestinationID() const { return shortestPathDestinationID; }

/**
 * Returns the shortest path based on time to reach one of the possible destination intersections from one of the source intersections.
 */
const std::vector<RoadSegment*> &DijkstraDirectedSP::getShortestPath() const { return shortestPath; }
