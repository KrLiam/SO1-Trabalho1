#pragma once

#include <vector>
#include <queue>

#include "pcb.h"

class Scheduler {
public:
	virtual ~Scheduler() {};
    virtual void insert(PCB& pcb) = 0;
    virtual PCB* pick() = 0;
    virtual bool test(PCB& pcb) = 0;
};


class SchedulerFCFS : public Scheduler {
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

	bool test(PCB&) {
		return false; // essa estrategia nao faz preempcao
	}
};

class SchedulerSJF : public Scheduler {
	class CompareDuration {
	public:
		bool operator()(PCB* a, PCB* b) {
			return a->duration > b->duration;
		}
	};
	// A estrutura priority queue permite a insercao ordenada de elementos pela duracao
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

class SchedulerPreemptivePriority : public Scheduler {
	class ComparePriority {
	public:
		bool operator()(PCB* a, PCB* b) {
			return a->priority < b->priority;
		}
	};
	// A estrutura priority queue permite a insercao ordenada de elementos pela prioridade
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

class SchedulerNonPreemptivePriority : public Scheduler {
	class ComparePriority {
	public:
		bool operator()(PCB* a, PCB* b) {
			return a->priority < b->priority;
		}
	};
	// A estrutura priority queue permite a insercao ordenada de elementos pela prioridade
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

	bool test(PCB&) {
		return false;  // essa estrategia nao faz preempcao
	}
};

class SchedulerRoundRobin : public Scheduler {
	int quantum;
	int counter = 0;
	std::queue<PCB*> queue;

public:

	SchedulerRoundRobin(int quantum) : quantum(quantum) {}

	void insert(PCB& pcb) {
		queue.push(&pcb);
	}

	PCB* pick() {
		if (!queue.size()) return NULL;
		counter = 0;  // Reinicia o contador durante uma preempcao ou quando um processo termina
		PCB* pcb = queue.front();
        queue.pop();
        return pcb;
	}

	bool test(PCB&) {
		counter++;
		if (!queue.size()) return false;  // Caso nao haja processos na fila de pronto, nao faz preempcao mesmo que o quantum tenha esgotado
		return counter >= quantum;
	}
};