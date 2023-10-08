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

    PCB(int id, int startTime, int duration, int priority, Context* context);

    bool finished();
};