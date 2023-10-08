#include "pcb.h"

PCB::PCB(int id, int startTime, int duration, int priority, Context* context)
        : id(id),
          startTime(startTime),
          endTime(-1),
          duration(duration),
          priority(priority),
          processState(pNew),
          executingTime(0),
          context(context) {}

bool PCB::finished() {
    return executingTime >= duration;
}
