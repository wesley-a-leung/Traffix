#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include <vector>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include "Point2D.h"
#include "Forward.h"
#include "RoadSegment.h"
#include "TrafficLight.h"
#include "../misc/pair_hash.h"

struct Intersection {
private:
    static int counter; // number of intersections that have been created
    int id; // each intersection has a unique id number
    bool leftTurn; // whether there is a left turn signal on
    double scheduledTime; // the next time the intersection is scheduled to be cycled, -1.0 if there is no time scheduled
    std::unordered_map<int, RoadSegment*> inboundRoads; // inbound road segments
    std::unordered_map<int, RoadSegment*> outboundRoads; // outbound road segments
    std::unordered_map<int, Intersection*> inboundIntersections; // intersections that the inbound roads come from
    std::unordered_map<int, Intersection*> outboundIntersections; // intersections that the outbound roads come from
    std::unordered_map<int, RoadSegment*> roadFrom; // maps the ID of the adjacent intersection to the road that lead from it
    std::unordered_map<int, RoadSegment*> roadTo; // maps the ID of the adjacent intersection to the road that lead to it
    std::unordered_map<int, std::unordered_set<int>> adjacentOut; // adjacency list of lights leading out from road segments
    std::unordered_map<int, std::unordered_set<int>> adjacentIn; // adjacency list of lights leading in to road segments
    std::unordered_map<std::pair<int, int>, TrafficLight*, pair_hash<int, int>> lights; // traffic lights between pair of road segments
    std::unordered_map<int, TrafficLight*> lightFromID; // maps the ID of the traffic light to the pair of road segments it connects
    Point2D location; // location of intersection in cartesian plane
    std::unordered_map<int, std::unordered_set<int>> linksLeft; // the left lights that are linked to a light
    std::unordered_map<int, std::unordered_set<int>> linksStraight; // the straight lights that are linked to a light
    std::unordered_map<int, std::unordered_set<int>> linksRight; // the right lights that are linked to a light
    int numberOfCycles; // number of cycles (sets of linked traffic lights) in the intersection
    int currentCycleNumber; // current cycle number of the intersection
    std::vector<std::unordered_set<int>> cycleToLight; // the set of lights associated with the cycle number
    std::unordered_map<int, int> cycleNumber; // cycle number of a light
    double timeOfLastCycle;

    // void dfs(int light, int cur);

public:
    Intersection(double x, double y);
    Intersection(Point2D &location);
    ~Intersection();
    int getID() const;
    bool add(RoadSegment *r);
    bool remove(RoadSegment *r);
    void connect(int from, int to, int type);
    void link(int A, int B);
    void autoConnectAndLink();
    // void assign();
    void cycle(double time);
    int getCurrentCycle() const;
    bool leftTurnSignalOn() const;
    int getCurrentFlow();
    int getOppositeFlow();
    double getTimeOfLastCycle() const;
    bool isScheduled() const;
    double getScheduledTime() const;
    void setScheduledTime(double time);
    void resetScheduledTime();
    int outdegree() const;
    int indegree() const;
    const std::unordered_map<int, RoadSegment*> &getInboundRoads() const;
    const std::unordered_map<int, RoadSegment*> &getOutboundRoads() const;
    const std::unordered_map<int, Intersection*> &getInboundIntersections() const;
    const std::unordered_map<int, Intersection*> &getOutboundIntersections() const;
    bool isConnected(int from, int to);
    TrafficLight *getLightBetween(int from, int to);
    const std::unordered_map<int, TrafficLight*> &getTrafficLights() const;
    TrafficLight *getLightFromID(int id);
    RoadSegment *getRoadFrom(int id);
    RoadSegment *getRoadTo(int id);
    Point2D getLocation() const;
    bool operator == (const Intersection &n) const;
    bool operator != (const Intersection &n) const;
};

#endif
