#pragma once

#include "pcb.h"

class SchedulingStrategy {
public:
    void newProcess(const PCB&);
    PCB* pick();
    bool test(PCB&);

    ~SchedulingStrategy() {}
};