#include <assert.h>
#include <cstdio>
#include "WeightedDigraph.h"

using namespace std;

/**
 * Initializes a Weighted Directed Graph.
 */
WeightedDigraph::WeightedDigraph() {
    intersections = 0;
    roadSegments = 0;
}

/**
 * Deconstructs the Weighted Directed Graph.
 */
WeightedDigraph::~WeightedDigraph() {}

/**
 * Returns the number of intersections (vertices) in this graph.
 */
int WeightedDigraph::countIntersections() const { return intersections; }

/**
 * Returns the number of road segments (edges) in this graph.
 */
int WeightedDigraph::countRoadSegments() const { return roadSegments; }

/**
 * Adds a road segment to the graph.
 * @param r the Road Segment
 * @return false if the road segment is already in the graph, true otherwise
 */
bool WeightedDigraph::addRoadSegment(RoadSegment *r) {
    if (idToRoadSegment.count(r->getID())) return false;
    idToRoadSegment[r->getID()] = r;
    if (idToIntersection.count(r->getSource()->getID()) == 0) intersections++;
    idToIntersection[r->getSource()->getID()] = r->getSource();
    if (idToIntersection.count(r->getDestination()->getID()) == 0) intersections++;
    idToIntersection[r->getDestination()->getID()] = r->getDestination();
    r->getSource()->add(r);
    r->getDestination()->add(r);
    compressedIndex[r->getID()] = roadSegments++;
    roadSegmentIDs.push_back(r->getID());
    return true;
}

/**
 * Splits a road segment into two parts.
 * @param r the Road Segment to split
 * @param n the intersection to split the Road Segment
 */
pair<RoadSegment*, RoadSegment*> WeightedDigraph::splitRoadSegment(RoadSegment *r, Intersection *n) {
    assert(removeRoadSegment(r->getID()));
    RoadSegment *a = new RoadSegment(r->getSource(), n, r->getSpeedLimit(), r->getCapacity());
    RoadSegment *b = new RoadSegment(n, r->getDestination(), r->getSpeedLimit(), r->getCapacity());
    addRoadSegment(a);
    addRoadSegment(b);
    return make_pair(a, b);
}

/**
 * Removes a road segment from the graph.
 * @param id the ID of the road segment to be removed to the graph
 * @return true if the road segment was removed, false if the id is not valid
 */
bool WeightedDigraph::removeRoadSegment(int id) {
    if (idToRoadSegment.count(id) == 0) return false;
    RoadSegment *r = idToRoadSegment[id];
    Intersection *source = r->getSource(), *destination = r->getDestination();
    source->remove(r);
    if (source->outdegree() == 0 && source->indegree() == 0) {
        idToIntersection.erase(source->getID());
        intersections--;
        delete source;
    }
    r->getDestination()->remove(r);
    if (destination->outdegree() == 0 && destination->indegree() == 0) {
        idToIntersection.erase(destination->getID());
        intersections--;
        delete destination;
    }
    idToRoadSegment.erase(id);
    if (roadSegmentIDs.size() > 1) {
        roadSegmentIDs[compressedIndex[id]] = roadSegmentIDs.back();
        compressedIndex[roadSegmentIDs.back()] = compressedIndex[id];
    }
    compressedIndex.erase(id);
    roadSegmentIDs.pop_back();
    roadSegments--;
    return true;
}

/**
 * Returns the number of outbound road segments in this intersection.
 * @param id the id of the intersection
 */
int WeightedDigraph::outdegree(int id) {
    assert(idToIntersection.count(id) && "there is no intersection with the given id");
    return idToIntersection[id]->outdegree();
}

/**
 * Returns the number of inbound road segments in this intersection.
 * @param id the id of the intersection
 */
int WeightedDigraph::indegree(int id) {
    assert(idToIntersection.count(id) && "there is no intersection with the given id");
    return idToIntersection[id]->indegree();
}

/**
 * Returns an immutable reference to the intersections (and their IDs).
 */
const unordered_map<int, Intersection*> &WeightedDigraph::getIntersections() const { return idToIntersection; }

/**
 * Returns a pointer to the intersection with the specified ID.
 */
Intersection* WeightedDigraph::getIntersection(int id) {
    assert(idToIntersection.count(id) > 0 && "there is no intersection with the given id");
    return idToIntersection[id];
}

/**
 * Returns an immutable reference to the road segments (and their IDs).
 */
const unordered_map<int, RoadSegment*> &WeightedDigraph::getRoadSegments() const { return idToRoadSegment; }

/**
 * Returns a pointer to the road segment with the specified ID.
 */
RoadSegment* WeightedDigraph::getRoadSegment(int id) {
    assert(idToRoadSegment.count(id) > 0 && "there is no road segment with the given id");
    return idToRoadSegment[id];
}

/**
 * Returns an immutable reference to the vector of road segment IDs.
 */
const vector<int> &WeightedDigraph::getRoadSegmentIDs() const { return roadSegmentIDs; }

/**
 * Returns the ID of the road segment given an index.
 */
int WeightedDigraph::getRoadSegmentID(int index) {
    assert(index >= 0 && index < roadSegments && "the index must statisfy 0 <= index < number of road segments");
    return roadSegmentIDs[index];
}

/**
 * Returns an immutable reference to compressed index of each road segment.
 */
const unordered_map<int, int> &WeightedDigraph::getCompressedIndices() const { return compressedIndex; }

/**
 * Returns the compressed index of the road segment given an ID.
 */
int WeightedDigraph::getCompressedIndex(int id) {
    assert(compressedIndex.count(id) > 0 && "there is no road segment with the given id");
    return compressedIndex[id];
}

/**
 * Returns the efficiency of the city.
 */
double WeightedDigraph::getEfficiency() { return Car::getEfficiency(); }
