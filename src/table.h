
#include <vector>

#include "pcb.h"

class ProcessTable {
    std::vector<PCB> processes;

public:
    void createProcess(int creationTime, int duration, int priority);

    PCB& getProcess(int id) {}

    vector<PCB&>& getByState(ProcessState);

    void changeState(PCB& aff, ProcessState newState);

    void clear();
};