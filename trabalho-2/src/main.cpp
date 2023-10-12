#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "simulator.h"

std::vector<int> read_input() {
    std::vector<int> result;

    int value;
    while (true) {
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

    std::vector<int> input = read_input();

    for (int x : input) {
        std::cout << x << " ";
    }
    std::cout << std::endl; 

    Simulator simulator;
    simulator.simulate(input);

    std::cout << "frames: " << frame_amount << std::endl;
}