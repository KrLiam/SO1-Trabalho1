
#include <vector>
#include <queue>

#include "file.h"
#include "pcb.h"
#include "table.h"
#include "strategy.h"

class Scheduler
{
    ProcessTable table;
    SchedulingStrategy schedule;
    PCB* activeProcess;

    Scheduler(SchedulingStrategy strategy) : schedule(strategy) {}

public:

    std::vector<int> simulate(std::vector<ProcessParams> processes) {
        table.clear();

        // preparar fila de processos
        // certifica que estao na ordem de criacao adequada	sort(processes.begin(), process.end(), [](ProcessParams a, ProcessParams b) {		return a.creationTime <= b.creationTime;	});
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
            for (PCB& process : table.getByState(pNew)) {
                table.changeState(process, pReady);
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

            // testa se o processo encerrou
            if (activeProcess->executingTime >= activeProcess->duration) {
                table.changeState(activeProcess, pFinished);
                activeProcess = NULL;
                continue;
            }

            // testa se o processo ativo deve ser preemptado
            if (schedule.test(activeProcess)) {
                table.changeState(activeProcess, pReady);
                activeProcess = NULL;
                continue;
            }
        }
    }
};
