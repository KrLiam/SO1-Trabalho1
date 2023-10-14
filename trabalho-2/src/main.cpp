#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "simulator.h"

std::vector<int> read_input() {
    std::vector<int> result;

    int value;
    while (!feof(stdin)) {
        std::cin >> value;
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

    int frame_amount = std::atoi(argv[1]);

    Simulator simulator(frame_amount);
    FIFO fifo(frame_amount);
    simulator.add_algorithm(fifo);

    simulator.simulate();
}