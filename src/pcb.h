#pragma once

#include "context.h"

enum ProcessState {pNew, pReady, pExecuting, pFinished};

struct PCB
{
    int id;
    int startTime;
    int endTime;
    int duration;
    int priority;
    ProcessState processState = pNew;
    int executingTime;
    Context* context;

    PCB(int id, int startTime, int duration, int priority, Context* context)
        : id(id),
          startTime(startTime),
          endTime(-1),
          duration(duration),
          priority(priority),
          processState(pNew),
          executingTime(0),
          context(context) {}

    bool finished() {
        return executingTime >= duration;
    }
};