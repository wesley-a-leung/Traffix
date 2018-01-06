#ifndef PRETIMEDCONTROLLER_H_
#define PRETIMEDCONTROLLER_H_

#include "../framework/Framework.h"
#include "Controller.h"

struct PretimedController : public Controller {
public:
    PretimedController(WeightedDigraph *G);
    ~PretimedController();
    void addEvent(double time, int id);
    bool checkNextEvent(double currentTime) const;
    void runEvents(double currentTime);
};

#endif
