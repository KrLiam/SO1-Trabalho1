#include <iostream>

#include "simulator.h"
#include "substitution_algorithm.h"


Simulator::Simulator(int frame_amount) : frame_amount(frame_amount) {}

void Simulator::add_algorithm(SubstitutionAlgorithm& algorithm) {
    algorithms.push_back(&algorithm);
}

void Simulator::optimal(std::vector<int>& accesses) {
    present_pages.clear();
    faults = 0;
    int count;
    int acc;
    int current_total = 0;
    int page_to_remove;

    for (std::size_t i = 0; i < accesses.size(); i++) {
        int page = accesses[i];
        // Página presente
        if (present_pages.count(page)){
            // std::cout << "page hit " << page << std::endl;
            continue;
        }
        // Página ausente mas há molduras livres
        // std::cout << "page fault " << page;
        int size = present_pages.size();
        if (size < frame_amount) {
            present_pages.insert(page);
            current_total += page;
            // std::cout << std::endl;
            continue;
        }
        // Página ausente e não há molduras livres
        faults++;
        count = 0;
        acc = 0;
        for (std::size_t j = i+1; j < accesses.size(); j++) {
            int look_ahead = accesses[j];
            if (present_pages.count(look_ahead)) {
                count++;
                acc += look_ahead;
                if (count == frame_amount - 1) break;
            }
        }
        page_to_remove = current_total - acc;
        // std::cout << ", replaced page " << page_to_remove << std::endl;
        present_pages.erase(page_to_remove);
        present_pages.insert(page);
        current_total = acc + page;

    }
    std::cout << "Ótimo: " << faults << " PFs" << std::endl;
}

void Simulator::simulate(std::vector<int>& accesses) {
    std::cout << frame_amount << " quadros" << std::endl
              << accesses.size() << " refs " << std::endl;

    // Chamar algoritmo ótimo
    optimal(accesses);
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