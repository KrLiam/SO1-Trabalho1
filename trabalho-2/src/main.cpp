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

int main(int argc, char const *argv[])
{
    // ./simulador 4 < referencias.txt
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <frame amount: integer>" << std::endl;
		return 1;
	}

    unsigned int frame_amount = std::atoi(argv[1]);

    Simulator simulator(frame_amount);
    FIFO fifo;
    LRU lru;
    simulator.add_algorithm(fifo);
    simulator.add_algorithm(lru);

    std::vector<page_t> input = read_input();
    simulator.simulate(input);
}