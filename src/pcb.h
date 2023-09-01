#pragma once

#include "state.h"
#include "context.h"

struct PCB{
    int id;
    int startTime;
    int endTime;
    int duration;
    int priority;
    ProcessState processState;
    int executingTime;
    Context context;

    PCB(int id, int startTime, int duration, int priority)
     : id(id),
       startTime(startTime),
       endTime(-1),
       duration(duration),
       priority(priority),
       processState(pNew),
       executingTime(0) {}
};