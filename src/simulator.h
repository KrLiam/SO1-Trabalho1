
#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>

#include "file.h"
#include "pcb.h"
#include "table.h"
#include "scheduler.h"

class Simulator
{
    ProcessTable table;
    Scheduler& schedule;
    PCB* activeProcess = NULL;

public:

    Simulator(Scheduler& strategy) : schedule(strategy) {}

    std::vector<int> simulate(std::vector<ProcessParams> processes) {
        table.clear();

        // preparar fila de processos
        // certifica que estao na ordem de criacao adequada
        std::sort(processes.begin(), processes.end(), [](ProcessParams a, ProcessParams b) {
            return a.creation_time < b.creation_time;
        });
        std::queue<ProcessParams>  creationQueue;
        for (ProcessParams p : processes) creationQueue.push(p);

        int time = 0;
        std::vector<int> result;

        while (1) {
            while (!creationQueue.empty()) {
                ProcessParams p = creationQueue.front();
                if (p.creation_time > time)
                    break;
                table.createProcess(p.creation_time, p.duration, p.priority);
                creationQueue.pop();
            }
            for (PCB* process : table.getByState(pNew)) {
                table.changeState(process, pReady);
                schedule.insert(*process);
            }

            // testa se o processo encerrou
            if (activeProcess && activeProcess->finished()) {
                table.changeState(activeProcess, pFinished);
                table.getProcess(activeProcess->id).endTime = time;
                activeProcess = NULL;
            }
            // se não encerrou, testa se o processo ativo deve ser preemptado
            else if (activeProcess && schedule.test(*activeProcess)) {
                // devolve processo para a estratégia
                table.changeState(activeProcess, pReady);
                schedule.insert(*activeProcess);
                activeProcess = NULL;
            }

            // escolhe um processo se nao há um processo ativo
            if (!activeProcess) {
                activeProcess = schedule.pick();
                // encerra se não há mais processos para serem executados
                if (!activeProcess) {
                    return result;
                }
            }

            time++;
            activeProcess->executingTime++;
            result.push_back(activeProcess->id);
        }
    }

    void print_graph(std::vector<int> result) {
        int total_processes = table.getProcessCount();
        int total_time = result.size();

        cout << "tempo ";
        for(int i = 0; i < total_processes; i++) {
            cout << "P" << i << " ";
        }
        cout << endl;
        for(int timestamp = 0; timestamp < total_time; timestamp++) {
            cout << left << setw(6) << to_string(timestamp) + "-" + to_string(timestamp+1);
            for (int pid = 0; pid < total_processes; pid++) {
                
                bool process_is_running_or_waiting = (table.getProcess(pid).startTime <= timestamp) && (table.getProcess(pid).endTime > timestamp);
                if (result[timestamp] == pid) {
                    cout << "## ";
                } else if (process_is_running_or_waiting) {
                    cout << "-- ";
                } else {
                    cout << "   ";
                }
            }
            cout << endl;
        }
    }
};
