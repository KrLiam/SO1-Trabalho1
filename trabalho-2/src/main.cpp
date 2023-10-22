#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "simulator.h"

std::vector<page_t> read_input() {
    std::vector<page_t> result;

    page_t value;
    while (std::cin >> value) {
        if (value < 0) break;
        result.push_back(value);
    }

    return result;
}

void benchmark(std::vector<page_t> input) {
    // open a file in write mode
    std::ofstream outfile;
    outfile.open("plotting/output.txt");
    for (unsigned int frame_amount = 1; frame_amount < 100; frame_amount++) {
        outfile << frame_amount << " ";
        Simulator simulator(frame_amount);
        FIFO fifo;
        LRU lru;
        simulator.add_algorithm(fifo);
        simulator.add_algorithm(lru);

        std::vector<int> faults = simulator.simulate(input);
        for (int fault : faults) {
            outfile << fault << " ";
        }
        outfile << std::endl;
    }
}

int main()
{
    // ./simulador 4 < referencias.txt
	// if (argc != 2) {
	// 	std::cout << "Usage: " << argv[0] << " <frame amount: integer>" << std::endl;
	// 	return 1;
	// }

    std::vector<page_t> input = read_input();
    // Simulator simulator(frame_amount);
    // FIFO fifo;
    // LRU lru;
    // simulator.add_algorithm(fifo);
    // simulator.add_algorithm(lru);
    // std::vector<int> faults = simulator.simulate(input);
    benchmark(input);
}

