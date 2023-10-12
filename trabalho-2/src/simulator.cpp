#include <iostream>

#include "simulator.h"
#include "substitution_algorithm.h"

Simulator::Simulator(int frame_amount) : frame_amount(frame_amount) {}


SimulationEntry::SimulationEntry(SubstitutionAlgorithm& algorithm)
    : algorithm(algorithm) {}


void Simulator::simulate() {
    std::vector<SimulationEntry> entries;

    for (SubstitutionAlgorithm* algorithm : algorithms) {
        entries.push_back(SimulationEntry(*algorithm));
    }

    while (true) {
        int page;
        std::cin >> page;
        if (page < 0) break;

        for (SimulationEntry entry : entries) {
            access(entry, page);
        }
    }
}

inline void Simulator::access(SimulationEntry& entry, int page) {
    if (present_pages.contains(page)) {
        
    }
}