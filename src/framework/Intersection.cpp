#include <assert.h>
#include <algorithm>
#include "Intersection.h"

#define PI 3.14159265358979323846
#define EPS 1e-9

using namespace std;

int Intersection::counter = 0; // counter starts at 0

/**
 * Initializes a new intersection given an x, y coordianate in the 2-D cartesian plane.
 * @param x the x-coordinate
 * @param y the y-coordinate
 */
Intersection::Intersection(double x, double y) {
    this->location = Point2D(x, y);
    id = Intersection::counter++; // assigns an id and increments the counter
    currentCycleNumber = 0;
    numberOfCycles = 0;
    leftTurn = true;
    scheduledTime = -1.0;
    timeOfLastCycle = 0.0;
}

/**
 * Initializes a new intersection given a point in the 2-D cartesian plane.
 * @param location the location of the intersection as a point
 */
Intersection::Intersection(Point2D &location) {
    this->location = Point2D(location.x, location.y);
    id = Intersection::counter++; // assigns an id and increments the counter
    currentCycleNumber = 0;
    numberOfCycles = 0;
    leftTurn = true;
    scheduledTime = -1.0;
    timeOfLastCycle = 0.0;
}

/**
 * Deconstructs the Intersection
 */
Intersection::~Intersection() {}

/**
 * Returns the unique ID of this intersection.
 */
int Intersection::getID() const { return id; }

/**
 * Adds a RoadSegment to the intersection
 * @return false if the road segment is already in the intersection, true otherwise
 */
bool Intersection::add(RoadSegment *r) {
    if (inboundRoads.count(r->getID()) || outboundRoads.count(r->getID()) ) return false;
    if (r->getDestination()->getID() == this->id) {
        inboundRoads[r->getID()] = r;
        inboundIntersections[r->getSource()->getID()] = r->getSource();
        roadFrom[r->getSource()->getID()] = r;
    }
    if (r->getSource()->getID() == this->id) {
        outboundRoads[r->getID()] = r;
        outboundIntersections[r->getDestination()->getID()] = r->getDestination();
        roadTo[r->getDestination()->getID()] = r;
    }
    if (r->getDestination()->getID() != this->id && r->getSource()->getID() != this->id) {
        assert(false && "this road segment does not start or end at this intersection");
    }
    return true;
}

/**
 * Removes a RoadSegment from the intersection
 * @return false if the road segment was not in the intersection, true otherwise
 */
bool Intersection::remove(RoadSegment *r) {
    if (inboundRoads.count(r->getID()) == 0 && outboundRoads.count(r->getID()) == 0) return false;
    if (inboundRoads.count(r->getID())) {
        inboundRoads.erase(r->getID());
        inboundIntersections.erase(r->getSource()->getID());
        roadFrom.erase(r->getSource()->getID());
        for (int to : adjacentOut[r->getID()]) { // removing all paths that lead out from the road segment
            // removing the linked traffic lights
            int lightID = lights[make_pair(r->getID(), to)]->getID();
            for (int link : linksLeft[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksLeft.erase(lightID);
            for (int link : linksStraight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksStraight.erase(lightID);
            for (int link : linksRight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksRight.erase(lightID);
            // removing the traffic light
            lightFromID.erase(lightID);
            int cycleNum = cycleNumber[lightID];
            cycleToLight[cycleNum].erase(lightID);
            if (cycleToLight[cycleNum].size() == 0) {
                for (int light : cycleToLight.back()) {
                    cycleToLight[cycleNum].insert(light);
                    cycleNumber[light] = cycleNum;
                }
                cycleToLight[cycleNum].erase(lightID);
                numberOfCycles--;
            }
            cycleNumber.erase(lightID);
            delete lights[make_pair(r->getID(), to)];
            lights.erase(make_pair(r->getID(), to));
            adjacentIn[to].erase(r->getID());
        }
        adjacentOut.erase(r->getID());
    }
    if (outboundRoads.count(r->getID())) { // removing all paths that lead into the road segment
        outboundRoads.erase(r->getID());
        outboundIntersections.erase(r->getDestination()->getID());
        roadTo.erase(r->getDestination()->getID());
        for (int from : adjacentIn[r->getID()]) {
            // removing the linked traffic lights
            int lightID = lights[make_pair(from, r->getID())]->getID();
            for (int link : linksLeft[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksLeft.erase(lightID);
            for (int link : linksStraight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksStraight.erase(lightID);
            for (int link : linksRight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksRight.erase(lightID);
            // removing the traffic light
            lightFromID.erase(lightID);
            int cycleNum = cycleNumber[lightID];
            cycleToLight[cycleNum].erase(lightID);
            if (cycleToLight[cycleNum].size() == 0) {
                for (int light : cycleToLight.back()) {
                    cycleToLight[cycleNum].insert(light);
                    cycleNumber[light] = cycleNum;
                }
                cycleToLight[cycleNum].erase(lightID);
                numberOfCycles--;
            }
            cycleNumber.erase(lightID);
            delete lights[make_pair(from, r->getID())];
            lights.erase(make_pair(from, r->getID()));
            adjacentOut[from].erase(r->getID());
        }
        adjacentIn.erase(r->getID());
    }
    return true;
}

/**
 * Connects two roads with a traffic light with a specified type
 * @param from the ID of the source road segment
 * @param to the ID of the destination road segment
 * @param type the type of turn the traffic light controls (0 for left, 1 for straight, 2 for right, 3 for u turn)
 */
void Intersection::connect(int from, int to, int type) {
    assert(inboundRoads.count(from) && "no inbound road exists in the intersection");
    assert(outboundRoads.count(to) && "no outbound road exists in the intersection");
    adjacentOut[from].insert(to);
    adjacentIn[to].insert(from);
    TrafficLight *t = new TrafficLight(inboundRoads[from], outboundRoads[to], type);
    lights[make_pair(from, to)] = t;
    lightFromID[t->getID()] = t;
    if (t->getType() == STRAIGHT) {
        cycleNumber[t->getID()] = numberOfCycles;
        cycleToLight.push_back(unordered_set<int>());
        cycleToLight[numberOfCycles].insert(t->getID());
        numberOfCycles++;
    }
}

/**
 * Links two lights together so they can be synchronized.
 * Lights can only be linked if A and B are both straight types, A is straight, B is left, or A is straight, B is right
 * @param A the ID of one traffic light
 * @param B the ID of the other traffic light
 */
void Intersection::link(int A, int B) {
    int AType = lightFromID[A]->getType();
    assert(AType == STRAIGHT && "light A must be of type straight");
    int BType = lightFromID[B]->getType();
    if (BType == LEFT || BType == UTURN) { // u turn lights will be treated as left turn lights
        linksLeft[A].insert(B);
        linksStraight[B].insert(A);
    } else if (BType == STRAIGHT) {
        linksStraight[A].insert(B);
        linksStraight[B].insert(A);
        int minCycle = min(cycleNumber[A], cycleNumber[B]);
        int maxCycle = max(cycleNumber[A], cycleNumber[B]);
        assert(minCycle != maxCycle && "lights are alread linked");
        cycleNumber[A] = cycleNumber[B] = minCycle;
        for (int light : cycleToLight[maxCycle]) {
            cycleToLight[minCycle].insert(light);
            cycleNumber[light] = minCycle;
        }
        cycleToLight[maxCycle].clear();
        for (int light : cycleToLight.back()) {
            cycleToLight[maxCycle].insert(light);
            cycleNumber[light] = maxCycle;
        }
        cycleToLight.pop_back();
        numberOfCycles--;
        assert(numberOfCycles == cycleToLight.size());
        assert(numberOfCycles > 0);
    } else { // BTYPE == RIGHT
        linksRight[A].insert(B);
        linksStraight[B].insert(A);
    }
}

/**
 * Iterates through all the road segements connected to this intersection and connects and links corresponding traffic lights;
 */
void Intersection::autoConnectAndLink() {
    unordered_map<int, int> outTypes;
    vector<RoadSegment*> outSorted;
    vector<pair<int, int>> straightLights;
    for (pair<int, RoadSegment*> out : outboundRoads) {
        outSorted.push_back(out.second);
    }
    sort(outSorted.begin(), outSorted.end(), RoadSegment::RoadSegmentPtrPolarOrderCmpGt);
    for (pair<int, RoadSegment*> in : inboundRoads) {
        outTypes.clear();
        Point2D inSource = in.second->getSource()->getLocation();
        Point2D inDest = in.second->getDestination()->getLocation();
        double inAngle = in.second->getDirection();
        double outAngle;
        double diffAngle;
        int closestDiffAngleID = -1;
        int straightLightID;
        double closestDiffAngle = 2 * PI;
        int startInd = lower_bound(outSorted.begin(), outSorted.end(), in.second, RoadSegment::RoadSegmentPtrPolarOrderCmpGt) - outSorted.begin();
        for (int i = 0; i < outSorted.size(); i++) { // uses the sorted vector so that right turns are more likely to be marked as such
            RoadSegment *out = outSorted[(startInd + i) % ((int) outSorted.size())];
            outAngle = out->getDirection(); 
            diffAngle = outAngle - inAngle;
            if (diffAngle < -PI) diffAngle += 2 * PI;
            if (diffAngle > PI) diffAngle -= 2 * PI;
            // u turns will be treated as straights if there are at most 1 other outbound roads
            if (abs(diffAngle) > PI - EPS) outTypes[out->getID()] = outSorted.size() <= 2 ? STRAIGHT : UTURN;
            else outTypes[out->getID()] = diffAngle > 0 ? RIGHT : LEFT;
            if (abs(diffAngle) < abs(closestDiffAngle) - EPS) {
                if (closestDiffAngleID != -1) outTypes[closestDiffAngleID] = abs(closestDiffAngle) > PI - EPS ?
                    (outSorted.size() <= 2 ? STRAIGHT : UTURN) : (closestDiffAngle > 0 ? RIGHT : LEFT);
                outTypes[out->getID()] = STRAIGHT;
                closestDiffAngle = diffAngle;
                closestDiffAngleID = out->getID();
            }
        }
        assert(closestDiffAngleID != -1);
        assert(outTypes.size() == outSorted.size());
        // connecting lights
        for (pair<int, int> p : outTypes) {
            connect(in.first, p.first, p.second);
            if (p.second == STRAIGHT) {
                straightLightID = lights[make_pair(in.first, p.first)]->getID();
                straightLights.push_back(make_pair(in.first, p.first));
            }
        }
        // linking straight lights with non straight lights
        for (pair<int, int> p : outTypes) {
            if (p.second != STRAIGHT) link(straightLightID, lights[make_pair(in.first, p.first)]->getID());
        }
    }
    // linkning straight lights with each other
    for (int i = 0; i < straightLights.size(); i++) {
        pair<int, int> a = straightLights[i];
        for (int j = i + 1; j < straightLights.size(); j++) {
            pair<int, int> b = straightLights[j];
            if ((inboundRoads[a.first]->getSource()->getID() == outboundRoads[b.second]->getDestination()->getID()
                    && outboundRoads[a.second]->getDestination()->getID() == inboundRoads[b.first]->getSource()->getID())
                    || (a.first == b.first))  {
                link(lights[a]->getID(), lights[b]->getID());
            }
        }
    }
}

/**
 * Helper function for assign. Ensures that all linked lights get assigned the same cycle number.
 */
/*void Intersection::dfs(int light, int cur) {
    cycleNumber[light] = cur;
    for (int next : linksStraight[light]) {
        if (cycleNumber[next] == -1) dfs(next, cur);
    }
}*/

/**
 * Assigns a cycle number for a light.
 */
/*void Intersection::assign() {
    // resets everything to negative 1
    for (pair<int, TrafficLight*> light : lightFromID) {
        cycleNumber[light.first] = -1;
    }
    int cur = 0;
    for (pair<int, TrafficLight*> light : lightFromID) {
        if (cycleNumber[light.first] == -1 && light.second->getType() == STRAIGHT) {
            dfs(light.first, cur);
            cur++;
        }
    }
    numberOfCycles = cur;
}*/

/**
 * Cycles the traffic lights in the intersection.
 */
void Intersection::cycle(double time) {
    resetScheduledTime();
    timeOfLastCycle = time;
    unordered_set<int> greens;
    unordered_set<int> lefts;
    // sets previous cycle to red
    for (int light : cycleToLight[(currentCycleNumber + numberOfCycles - 1) % numberOfCycles]) {
        if (lightFromID[light])
        lightFromID[light]->setState(RED);
        for (int left : linksLeft[light]) {
            lightFromID[left]->setState(RED);
        }
    }
    // sets current cycle to red (to start)
    for (int light : cycleToLight[currentCycleNumber]) {
        lightFromID[light]->setState(RED);
        for (int left : linksLeft[light]) {
            lightFromID[left]->setState(RED);
        }
    }
    // determins which lights might need to be set to green
    for (int light : cycleToLight[currentCycleNumber]) {
        if (lightFromID[light]->getType() == STRAIGHT) greens.insert(light);
        if (!leftTurn) {
            for (int left : linksLeft[light]) {
                lefts.insert(left);
            }
        }
    }
    assert(greens.size() > 0);
    // if there are left turns that need to be set to green, then all the left turn lights are set to green
    // otherwise, only the straight lights are set to green
    if (lefts.size() == 0) {
        leftTurn = false;
        for (int light : greens) {
            lightFromID[light]->setState(GREEN);
        }
        currentCycleNumber = (currentCycleNumber + 1) % numberOfCycles;
    } else {
        leftTurn = true;
        for (int light : lefts) {
            lightFromID[light]->setState(GREEN);
        }
    }
}

/**
 * Returns the current cycle number in the intersection.
 */
int Intersection::getCurrentCycle() const { return currentCycleNumber; }

/**
 * Returns true if any left turn signal is on, false otherwise.
 */
bool Intersection::leftTurnSignalOn() const { return leftTurn; }

/**
 * Returns true if the intersection is scheduled to cycle, false otherwise.
 */
bool Intersection::isScheduled() const { return scheduledTime - EPS > -1.0; }

/**
 * Returns the time the intersection is scheduled to cycle, -1.0 if it is not currently scheduled.
 */
double Intersection::getScheduledTime() const { return scheduledTime; }

/**
 * Sets the scheduled time the intersection is schedued to cycle to the specified time.
 */
void Intersection::setScheduledTime(double time) { scheduledTime = time; }

/**
 * Resets the scheduled time the intersection is schedued to cycle to 0.
 */
void Intersection::resetScheduledTime() { scheduledTime = -1.0; } 

/**
 * Returns the flow of vehicles in incoming roads that are green.
 */
int Intersection::getCurrentFlow() {
    int flow = 0;
    for (int t : cycleToLight[currentCycleNumber]) {
        flow += lightFromID[t]->getFrom()->getFlow();
    }
    return flow;
}

/**
 * Returns the flow of vehicles in incoming roads that are red.
 */
int Intersection::getOppositeFlow() {
    int flow = 0;
    for (int i = 0; i < numberOfCycles; i++) {
        if (i == currentCycleNumber) continue;
        for (int t : cycleToLight[i]) {
            flow += lightFromID[t]->getFrom()->getFlow();
        }
    }
    return flow;
}

/**
 * Returns the time since the last cycle change.
 */
double Intersection::getTimeOfLastCycle() const { return timeOfLastCycle; }

/**
 * Returns the number of outbound road segments in this intersection.
 */
int Intersection::outdegree() const { return outboundRoads.size(); }

/**
 * Returns the number of inbound road segments in this intersection.
 */
int Intersection::indegree() const { return inboundRoads.size(); }

/**
 * Returns an immutable reference to the road segments (and their IDs) coming into the interesection.
 */
const unordered_map<int, RoadSegment*> &Intersection::getInboundRoads() const { return inboundRoads; }

/**
 * Returns an immutable reference to the road segments (and their IDs) going out from the intersection.
 */
const unordered_map<int, RoadSegment*> &Intersection::getOutboundRoads() const { return outboundRoads; }

/**
 * Returns an immutable reference to the adjacent intersections (and their IDs) that have a road leading into this interesection.
 */
const unordered_map<int, Intersection*> &Intersection::getInboundIntersections() const { return inboundIntersections; }

/**
 * Returns an immutable reference to the adjacent intersections (and their IDs) that have a road leading out from this intersection.
 */
const unordered_map<int, Intersection*> &Intersection::getOutboundIntersections() const { return outboundIntersections; }

/**
 * Returns true if there is a traffic light between the two road segmetns given their IDs, false otherwise.
 */
bool Intersection::isConnected(int from, int to) {
    assert(inboundRoads.count(from) && outboundRoads.count(to) && "one of the roads is not in the intersection");
    pair<int, int> p = make_pair(from, to);  
    return lights.count(p) > 0;
}

/**
 * Returns a pointer to the traffic light between the two road segments given their IDs.
 */
TrafficLight *Intersection::getLightBetween(int from, int to) {
    assert(inboundRoads.count(from) && outboundRoads.count(to) && "one of the roads is not in the intersection");
    pair<int, int> p = make_pair(from, to);
    assert(lights.count(p) && "there is no light between the two roads");
    return lights[p];
}

/**
 * Returns an immutable reference to the traffic lights (and their IDs) in this intersection.
 */
const unordered_map<int, TrafficLight*> &Intersection::getTrafficLights() const { return lightFromID; }

/**
 * Returns a pointer to the traffic light given an id.
 */
TrafficLight *Intersection::getLightFromID(int id) {
    assert(lightFromID.count(id) && "there is no light in this intersection with the specified ID");
    return lightFromID[id];
}

/**
 * Returns a pointer to the road that leads from the intersection with the specified ID.
 */
RoadSegment *Intersection::getRoadFrom(int id) {
    assert(inboundIntersections.count(id) && "there is no road leading from this intersection");
    return roadFrom[id];
}

/**
 * Returns a pointer to the road that leads to the intersection with the specified ID.
 */
RoadSegment *Intersection::getRoadTo(int id) {
    assert(outboundIntersections.count(id) && "there is no road leading from this intersection");
    return roadTo[id];
}

/**
 * Returns the location of this interesection.
 */
Point2D Intersection::getLocation() const { return this->location; }

/**
 * Compares this Intersection to Intersection n by comparing their ids.
 *
 * @param  n the other intersection
 * @return true if and only if this intersection equals the other, false otherwise
 */
bool Intersection::operator == (const Intersection &n) const { return id == n.id; }

/**
 * Compares this Intersection to Intersection t by comparing their ids.
 *
 * @param  n the other intersection
 * @return true if and only if this intersection does not equals the other, false otherwise
 */
bool Intersection::operator != (const Intersection &n) const { return id != n.id; }
