#include <iostream>

#include "file.h"
#include "scheduler.h"
#include "strategy.h"

int main()
{
	File f;
	f.read_file();
	std::vector<ProcessParams> all_processes = f.get_processes();

	RoundRobin strategy;
	Scheduler scheduler(strategy);

	std::vector<int> result = scheduler.simulate(all_processes);
	scheduler.print_graph(result);


	std::cout << "[";
	for (int id : result) {
		std::cout << id << ", ";
	}
	std::cout << "]" << std::endl;
}