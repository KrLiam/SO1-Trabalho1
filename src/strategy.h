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

class SJF : public SchedulingStrategy {
	class CompareDuration {
	public:
		bool operator()(PCB* a, PCB* b) {
			return a->duration > b->duration;
		}
	};
	std::priority_queue<PCB*, std::vector<PCB*>, CompareDuration> queue;

public:
	void insert(PCB& pcb) {
		queue.push(&pcb);
	}

	PCB* pick() {
		if (!queue.size()) return NULL;
		PCB* pcb = queue.top();
		queue.pop();
		return pcb;
	}

	bool test(PCB& pcb) {
		if (!queue.size()) return false;
		PCB* potential_next_process = queue.top();
		return pcb.duration > potential_next_process->duration;
	}
};

class PreemptivePriority : public SchedulingStrategy {
	class ComparePriority {
	public:
		bool operator()(PCB* a, PCB* b) {
			return a->priority < b->priority;
		}
	};
	std::priority_queue<PCB*, std::vector<PCB*>, ComparePriority> queue;

public:
	void insert(PCB& pcb) {
		queue.push(&pcb);
	}

	PCB* pick() {
		if (!queue.size()) return NULL;
		PCB* pcb = queue.top();
		queue.pop();
		return pcb;
	}

	bool test(PCB& pcb) {
		if (!queue.size()) return false;
		PCB* potential_next_process = queue.top();
		return pcb.priority < potential_next_process->priority;
	}
};

class NonPreemptivePriority : public SchedulingStrategy {
	std::queue<PCB*> queue;
public:
	void insert(PCB& pcb) {
		return;
	}

	PCB* pick() {
	return NULL;
	}

	bool test(PCB& pcb) {
		return false;
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
		return quantum >= 2;
	}
};