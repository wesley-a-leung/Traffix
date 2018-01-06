#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <functional>
#include <utility>
#include <vector>
#include <queue>
#include "../framework/Framework.h"

struct Controller {
protected:
    WeightedDigraph *G; // the weighted directed graph, representing the city
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<std::pair<double, int>>> events;

public:
    Controller(WeightedDigraph *G);
    ~Controller();
    WeightedDigraph *getGraph() const;
    virtual void addEvent(double time, int id) = 0;
    virtual bool checkNextEvent(double currentTime) const = 0;
    virtual void runEvents(double currentTime) = 0;
};

#endif
