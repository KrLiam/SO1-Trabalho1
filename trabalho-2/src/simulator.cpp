#include <iostream>

#include "simulator.h"
#include "substitution_algorithm.h"


Simulator::Simulator(int frame_amount) : frame_amount(frame_amount) {}

void Simulator::add_algorithm(SubstitutionAlgorithm& algorithm) {
    algorithms.push_back(&algorithm);
}

void Simulator::simulate(std::vector<int>& accesses) {
    std::cout << frame_amount << " quadros" << std::endl
              << accesses.size() << " refs " << std::endl;

    for (SubstitutionAlgorithm* algorithm : algorithms) {
        present_pages.clear();
        faults = 0;

        for (int page : accesses) {
            if (present_pages.count(page)) {
                // std::cout << "page hit " << page << std::endl;
                algorithm->accessed(page);
            }
            else {
                faults++;
                // std::cout << "page fault " << page;
                int page_to_remove = algorithm->replace(page);
                if (page_to_remove >= 0) {
                    present_pages.erase(page_to_remove);
                    // std::cout << ", replaced page " << page_to_remove;
                }
                // std::cout << std::endl;

                present_pages.insert(page);
            }
        }

        std::cout << algorithm->name() << ": " << faults << " PFs" << std::endl;
    }    
}