#include "file.h"
#include "test.h"

int main()
{
	File f;
	f.read_file();
	vector<ProcessParams*> all_processes = f.get_processes();
	// sort the vector by arrival time
	sort(all_processes.begin(), all_processes.end(), [](ProcessParams* a, ProcessParams* b) {
		return a->creation_time < b->creation_time;
		});
	Scheduler scheduler = Scheduler();
	vector<int> result = scheduler.simulate(all_processes);
	scheduler.print_graph(result, 10, 4);
}