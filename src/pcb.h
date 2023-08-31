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
};