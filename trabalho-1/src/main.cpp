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
	
	// Instancia uma subclasse de Scheduler de acordo com o argumento passado
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
		scheduler = new SchedulerRoundRobin(2);
		break;
	default:
		std::cout << "Invalid scheduler strategy" << std::endl;
		return 1;
	}

	Simulator<ContextINE5412> simulator(*scheduler);

	simulator.simulate(all_processes);  // Armazena o resultado internamente para posteriormente imprimir o grafico

	simulator.print_graph();
	simulator.show_data();

	delete scheduler;
	return 0;
}