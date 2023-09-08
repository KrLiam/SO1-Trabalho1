#include "file.h"
#include "test.h"

int main()
{
	File f;
	f.read_file();
	vector<ProcessParams*> all_processes = f.get_processes();
	Scheduler scheduler = Scheduler();
	vector<int> result = scheduler.simulate(all_processes);
	scheduler.print_graph(result, 10, 4);
}