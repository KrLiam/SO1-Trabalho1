#pragma once

#include <vector>
#include <unordered_map>

#include "pcb.h"

class ProcessTable {
    std::vector<PCB> processes;

    std::unordered_map<ProcessState, std::vector<PCB*>> stateMap;

    int currentId = 0;

    int getNextId() {
        return ++currentId;
    }

    std::vector<PCB*>& getCreateByState(ProcessState state) {
        if (stateMap.count(state) <= 0) {
            stateMap.emplace(state, std::vector<PCB*>());
        }
        return stateMap.at(state);
    }

public:
    void createProcess(int creationTime, int duration, int priority) {
        PCB& process = processes.emplace_back(getNextId(), creationTime, duration, priority);
        changeState(&process, pNew);
    }

    PCB& getProcess(int id);

    const std::vector<PCB*>& getByState(ProcessState state);

    void changeState(PCB* process, ProcessState state) {
        // remove processo da lista do estado anterior
        std::vector<PCB*>& prevState = getCreateByState(process->processState);
        auto it = std::find(prevState.begin(), prevState.end(), process);
        if (it != prevState.end()) {
            prevState.erase(it);
        }
        // adiciona processo na lista do estado novo
        std::vector<PCB*>& newState = getCreateByState(state);
        auto newIt = std::find(newState.begin(), newState.end(), process);
        if (newIt == newState.end()) {
            newState.push_back(process);
        }
        
        process->processState = state;
    }
    void changeState(PCB& process, ProcessState state);

    void clear();
};