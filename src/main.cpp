#include <iostream>

#include "file.h"
#include "scheduler.h"
#include "strategy.h"

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
	
	SchedulingStrategy* strategy;
	switch (strategy_name)
	{
	case 'f':
		strategy = new FCFS();
		break;
	case 'r':
		strategy = new RoundRobin();
		break;
	default:
		cout << "Invalid strategy" << endl;
		return 1;
	}

	
	Scheduler scheduler(*strategy);

	std::vector<int> result = scheduler.simulate(all_processes);
	scheduler.print_graph(result);


	std::cout << "[";
	for (int id : result) {
		std::cout << id << ", ";
	}
	std::cout << "]" << std::endl;
}