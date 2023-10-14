#include <iostream>

#include "simulator.h"
#include "substitution_algorithm.h"


Simulator::Simulator(unsigned int frame_amount) : frame_amount(frame_amount) {}

void Simulator::add_algorithm(SubstitutionAlgorithm& algorithm) {
    algorithm.set_frame_amount(frame_amount);
    algorithms.push_back(&algorithm);
}

void Simulator::optimal(std::vector<int>& accesses) {
    present_pages.clear();
    // std::unordered_set<int> can_remove;

    faults = 0;
    int page_to_remove;

    for (std::size_t i = 0; i < accesses.size(); i++) {
        int page = accesses[i];
        // Página presente
        if (present_pages.count(page)){
            // std::cout << "page hit " << page << std::endl;
            continue;
        }

        // Pagina ausente
        faults++;
        // std::cout << "page fault " << page;

        // Há molduras livress
        if (present_pages.size() < frame_amount) {
            present_pages.insert(page);
            // std::cout << std::endl;
            continue;
        }

        // Não há molduras livres

        std::unordered_set<int> can_remove(present_pages);
        // recriar o unordered_set a cada iteração leva o msm tempo q
        // manter uma instancia fixa e fazer essa gambiarra ai embaixo
        // can_remove.clear();
        // can_remove.insert(present_pages.begin(), present_pages.end());

        for (std::size_t j = i+1; j < accesses.size(); j++) {
            int look_ahead = accesses[j];
            if (can_remove.count(look_ahead)) {
                can_remove.erase(look_ahead);

                if (can_remove.size() == 1) break;
            }
        }

        page_to_remove = *can_remove.begin();

        // std::cout << ", replaced page " << page_to_remove << ", frames:";
        present_pages.erase(page_to_remove);
        present_pages.insert(page);

        // for (int present : present_pages) {
        //     std::cout << " " << present;
        // }
        // std::cout << std::endl;
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