
#pragma once

#include <vector>
#include <queue>
#include <algorithm>

#include "file.h"
#include "pcb.h"
#include "table.h"
#include "scheduler.h"
#include <iomanip>

using namespace std;

class Simulator
{
    ProcessTable table;
public:
    void print_graph(vector<int> result, int total_time, int total_processes) {
        table.createProcess(0, 1, 0);
        table.createProcess(1, 1, 1);
        table.createProcess(1, 5, 1);
        table.createProcess(1, 1, 1);
        cout << "tempo ";
        for(int i = 0; i < total_processes; i++) {
            cout << "P" << i << " ";
        }
        cout << endl;
        for(int timestamp = 0; timestamp < total_time; timestamp++) {
            cout << left << setw(6) << to_string(timestamp) + "-" + to_string(timestamp+1);
            for (int pid = 0; pid < total_processes; pid++) {
                
                bool process_is_running_or_waiting = (table.getProcess(pid).startTime <= timestamp) && (table.getProcess(pid).startTime + table.getProcess(pid).duration > timestamp);
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

    std::vector<int> simulate(std::vector<ProcessParams*> processes) {
        vector<int>* result = new vector<int>();
        // for(ProcessParams* process: processes) {
        //     result->push_back(process->creation_time);
        // }
        result->push_back(-1); // 0
        result->push_back(-1); // 1
        result->push_back(-1); // 2
        result->push_back(2); // 3
        result->push_back(-1); // 4
        result->push_back(-1); // 5
        result->push_back(-1); // 6
        result->push_back(-1); // 7
        result->push_back(-1); // 8
        result->push_back(-1); // 9

        return *result;
    }
};
