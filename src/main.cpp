#include <iostream>

#include "file.h"
#include "scheduler.h"
#include "simulator.h"

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <strategy: f|s|p|n|r>" << std::endl;
		return 1;
	}
	File f;
	f.read_file();
	std::vector<ProcessParams> all_processes = f.get_processes();

	if (!all_processes.size()) return 1;

	char scheduler_name = argv[1][0];
	
	Scheduler* scheduler;
	switch (scheduler_name)
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
		std::cout << "Invalid scheduler strategy" << std::endl;
		return 1;
	}

	Simulator<ContextMIPS> simulator(*scheduler);

	simulator.simulate(all_processes);

	simulator.print_graph();

	simulator.show_data();

	delete scheduler;
	return 0;
}