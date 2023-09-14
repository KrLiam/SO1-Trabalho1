
#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>

#include "file.h"
#include "pcb.h"
#include "table.h"
#include "scheduler.h"

template <typename C>
class Simulator
{
    ProcessTable<C> table;
    Scheduler& scheduler;
    PCB* activeProcess = NULL;
    std::vector<int> result;
    
    C activeContext;

public:

    void show_context_table() {
        std::cout << "Active: ";
        activeContext.show();

        for (const PCB& pcb : table.getAllProcesses()) {
            std::cout << "P" << pcb.id << ": ";
            pcb.context->show();
        }
    }

    void switch_context(PCB* next_process) {
        #if DEBUG
        show_context_table();
        std::cout << "--- switching context. ";
        if (activeProcess) {
            std::cout << "saving active to " << activeProcess->id << ". ";
        }
        if (next_process) {
            std::cout << "loading " << next_process->id << " to active.";
        }
        std::cout << std::endl;
        #endif


        if (activeProcess) {
            table.saveContext(activeProcess, activeContext);
        } 
        if (next_process) {
            activeContext = table.loadContext(next_process);
        }
        activeProcess = next_process;


        #if DEBUG
        show_context_table();
        std::cout << std::endl;
        #endif
    }
    
    Simulator(Scheduler& strategy) : scheduler(strategy) {}

    void simulate(std::vector<ProcessParams> processes) {
        table.clear();
        result.clear();

        // preparar fila de processos
        // certifica que estao na ordem de criacao adequada
        std::sort(processes.begin(), processes.end(), [](ProcessParams a, ProcessParams b) {
            return a.creation_time < b.creation_time;
        });
        std::queue<ProcessParams>  creationQueue;
        for (ProcessParams p : processes) creationQueue.push(p);
        int time = 0;

        // Simulação, cada iteração é um segundo
        while (1) {
            // Checa quais processos devem ser criados no segundo atual
            while (!creationQueue.empty()) {
                ProcessParams p = creationQueue.front();
                if (p.creation_time > time)
                    break;
                table.createProcess(p.creation_time, p.duration, p.priority);
                creationQueue.pop();
            }
            // Envia os processos criados para o escalonador organizá-los
            for (PCB* process : table.getByState(pNew)) {
                table.changeState(process, pReady);
                scheduler.insert(*process);
            }

            bool shouldSwitch = false;
            if (activeProcess) {
                // Testa se o processo encerrou
                if (activeProcess->finished()) {
                    table.changeState(activeProcess, pFinished);
                    table.getProcess(activeProcess->id).endTime = time;
                    shouldSwitch = true;
                } else if (scheduler.test(*activeProcess)) {
                    // devolve processo para a estratégia
                    table.changeState(activeProcess, pReady);
                    scheduler.insert(*activeProcess);
                    shouldSwitch = true;
                }

            } 

            // escolhe um processo se nao há um processo ativo
            if (shouldSwitch || !activeProcess) {
                switch_context(scheduler.pick());
            }

            time++;
            if (!activeProcess) {
                if (creationQueue.empty()) {
                    break;
                } else {
                    result.push_back(-1);
                    continue;
                }
            }
            activeProcess->executingTime++;
            activeContext.tick(activeProcess->id);
            result.push_back(activeProcess->id);
        }
    }

    std::vector<int> get_result() {
        return result;
    }

    void print_graph() {
        int total_processes = table.getProcessCount();
        int total_time = result.size();

        std::cout << "tempo ";
        for(int i = 0; i < total_processes; i++) {
            std::cout << "P" << i << " ";
        }
        std::cout << std::endl;
        for(int timestamp = 0; timestamp < total_time; timestamp++) {
            std::cout << left << std::setw(6) << std::to_string(timestamp) + "-" + std::to_string(timestamp+1);
            for (int pid = 0; pid < total_processes; pid++) {
                
                PCB& process = table.getProcess(pid);
                bool process_is_running_or_waiting = (process.startTime <= timestamp) && (process.endTime > timestamp);
                if (result[timestamp] == pid) {
                    std::cout << "## ";
                } else if (process_is_running_or_waiting) {
                    std::cout << "-- ";
                } else {
                    std::cout << "   ";
                }
            }
            std::cout << std::endl;
        }
    }

    void show_data() {
        int context_changes = result.size() > 0;
        for (std::size_t i = 1; i < result.size(); i++) {
            if (result[i] != result[i-1]) {
                context_changes++;
            }
        }
        
        float average_wait_time = .0;
        for (PCB p : table.getAllProcesses()) {
            average_wait_time += p.endTime - p.startTime - p.duration;
        }
        average_wait_time /= table.getProcessCount();
        
        float average_turnaround_time = .0;
        for (PCB p : table.getAllProcesses()) {
            std::cout << "Processo " << p.id << " - Turnaround time: " << p.endTime - p.startTime << std::endl;
            average_turnaround_time += p.endTime - p.startTime;
        }
        average_turnaround_time /= table.getProcessCount();

        std::cout << "Tempo total de execução: " << result.size() << std::endl;
        std::cout << "Número de mudanças de contexto: " << context_changes << std::endl;
        std::cout << "Tempo médio de espera: " << average_wait_time << std::endl;
        std::cout << "Tempo médio de turnaround: " << average_turnaround_time << std::endl;
    }
};
