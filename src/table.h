#pragma once

#include <vector>

#include "pcb.h"

class ProcessTable {
    std::vector<PCB> processes;

public:
    void createProcess(int creationTime, int duration, int priority);

    PCB& getProcess(int id);

    std::vector<PCB*>& getByState(ProcessState state);

    void changeState(PCB* process, ProcessState newState) {
        changeState(*process, newState);
    }
    void changeState(PCB& process, ProcessState newState);

    void clear();
};