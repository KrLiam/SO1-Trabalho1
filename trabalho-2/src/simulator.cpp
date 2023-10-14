#include <iostream>

#include "simulator.h"
#include "substitution_algorithm.h"

SimulationEntry::SimulationEntry(SubstitutionAlgorithm& algorithm)
    : algorithm(algorithm) {}


Simulator::Simulator(int frame_amount) : frame_amount(frame_amount) {}

void Simulator::add_algorithm(SubstitutionAlgorithm& algorithm) {
    algorithms.push_back(&algorithm);
}

void Simulator::simulate() {
    std::vector<SimulationEntry> entries;

    for (SubstitutionAlgorithm* algorithm : algorithms) {
        entries.push_back(SimulationEntry(*algorithm));
    }

    int refs = 0;

    int page;
    while (std::cin >> page) {
        if (page < 0) break;

        refs++;
        for (SimulationEntry& entry : entries) {
            access(entry, page);
        }
    }

    std::cout << frame_amount << " quadros" << std::endl
              << refs << " refs " << std::endl;

    for (SimulationEntry& entry : entries) {
        std::cout << entry.algorithm.name() << ": " << entry.faults << " PFs" << std::endl;
    }
}

inline void Simulator::access(SimulationEntry& entry, int page) {
    if (entry.present_pages.count(page)) {
        // std::cout << "page hit " << page << std::endl;
        entry.algorithm.accessed(page);
    }
    else {
        entry.faults++;
        // std::cout << "page fault " << page;
        int page_to_remove = entry.algorithm.replace(page);
        if (page_to_remove >= 0) {
            entry.present_pages.erase(page_to_remove);
            // std::cout << ", replaced page " << page_to_remove;
        }
        // std::cout << std::endl;

        entry.present_pages.insert(page);
    }
}