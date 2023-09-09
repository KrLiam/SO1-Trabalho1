#include <iostream>

#include "file.h"
#include "simulator.h"
#include "scheduler.h"

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		std::cout << "Usage: ./main <strategy>" << std::endl;
		return 1;
	}
	File f;
	f.read_file();
	std::vector<ProcessParams> all_processes = f.get_processes();

	char strategy_name = argv[1][0];
	
	Scheduler* scheduler;
	switch (strategy_name)
	{
	case 'f':
		scheduler = new SchedulerFCFS();
		break;
	case 's':
		scheduler = new SchedulerSJF();
		break;
	case 'p':
		scheduler = new SchedulerPreemptivePriority();
		break;
	case 'n':
		scheduler = new SchedulerNonPreemptivePriority();
		break;
	case 'r':
		scheduler = new SchedulerRoundRobin();
		break;
	default:
		cout << "Invalid strategy" << endl;
		return 1;
	}

	
	Simulator simulator(*scheduler);

	std::vector<int> result = simulator.simulate(all_processes);
	simulator.print_graph(result);


	std::cout << "[";
	for (int id : result) {
		std::cout << id << ", ";
	}
	std::cout << "]" << std::endl;
}