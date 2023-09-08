#include <vector>
#include <queue>
#include <algorithm>

#include "file.h"
#include "pcb.h"
#include "table.h"
#include "strategy.h"
#include "scheduler.h"


class Simulator{
    ProcessTable table;
    Scheduler scheduler;
    PCB* activeProcess;
    int currentTime = 0;

    
public:
    Simulator(std::string strategy) {
        scheduler = Scheduler(strategy);
    };
    
    ~Simulator();

    std::vector<int> simulate(const std::vector<ProcessParams>& processes) {
        table.clear();

        // preparar fila de processos
        // certifica que estao na ordem de criacao adequada
        std::sort(processes.begin(), processes.end(), [](ProcessParams a, ProcessParams b) {
            return a.creation_time <= b.creation_time;
        });
        std::queue<ProcessParams>  creationQueue;
        for (ProcessParams p : processes) creationQueue.push(p);

        currentTime = 0;
        std::vector<int> result;

        while (1) {
            while (!creationQueue.empty()) {
                ProcessParams p = creationQueue.front();
                if (p.creation_time > currentTime)
                    break;
                table.createProcess(p.creation_time, p.duration, p.priority);
                creationQueue.pop();
            }
            for (PCB* process : table.getByState(pNew)) {
                table.changeState(process, pReady);
            }
            nextProcess = scheduler.pick(activeProcess);
            if (nextProcess != activeProcess) {
                // todo
                save_context(activeProcess);
                activeProcess = nextProcess;
            }
            
            // Rascunho do método do escalonador
            if (activeProcess == nullptr) {
                activeProcess = scheduler.nextProcess();
            } else if (test(activeProcess)) {
                table.changeState(activeProcess, pReady);
                activeProcess = NULL;
                continue;
            }



            currentTime++;
            activeProcess->executingTime++;
            result.push_back(activeProcess->id);

            // testa se o processo encerrou
            if (activeProcess->executingTime >= activeProcess->duration) {
                table.changeState(activeProcess, pFinished);
                activeProcess = NULL;
                continue;
            }
        }
    }};