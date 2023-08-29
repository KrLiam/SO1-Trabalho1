
#include "pcb.h"

class SchedulingStrategy {
public:
    void newProcess(const PCB&);
    PCB* pick();
    void test(PCB&);

    ~SchedulingStrategy() {}
};