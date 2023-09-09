#pragma once

#include <vector>

#include "pcb.h"

class SchedulingStrategy {
public:
    virtual void insert(PCB& pcb) = 0;
    virtual PCB* pick() = 0;
    virtual bool test(PCB& pcb) = 0;
};


class FCFS : public SchedulingStrategy {
	std::queue<PCB*> queue;

public:
	void insert(PCB& pcb) {
		queue.push(&pcb);
	}

	PCB* pick() {
		if (!queue.size()) return NULL;
		PCB* pcb = queue.front();
        queue.pop();
        return pcb;
	}

	bool test(PCB& pcb) {
		return false; // never preempts
	}
};


class RoundRobin : public SchedulingStrategy {
	std::queue<PCB*> queue;
	int quantum = 0;

public:

	void insert(PCB& pcb) {
		queue.push(&pcb);
	}

	PCB* pick() {
		if (!queue.size()) return NULL;

		quantum = 0;
		PCB* pcb = queue.front();
        queue.pop();
        return pcb;
	}

	bool test(PCB& pcb) {
		quantum++;

		if (quantum >= 2) {
			queue.push(&pcb);
			return true;
		}
		return false;
	}
};