#include <iostream>
#include <unordered_map>
#include <set>
#include <list>

#include "simulator.h"
#include "substitution_algorithm.h"


Simulator::Simulator(unsigned int frame_amount) : frame_amount(frame_amount) {}

void Simulator::add_algorithm(SubstitutionAlgorithm& algorithm) {
    algorithm.set_frame_amount(frame_amount);
    algorithms.push_back(&algorithm);
}

void Simulator::optimal(std::vector<int>& accesses) {
    int max_i = accesses.size();

    std::unordered_map<int, std::list<int>> uses;
    for (int i = max_i - 1; i >= 0; i--) {
        int page = accesses[i];

        if (!uses.count(page)) {
            uses.emplace(page, std::list<int>({max_i}));
        }
        uses.at(page).push_front(i);
    }

    present_pages.clear();

    faults = 0;
    int furthest_page = -1;
    int furthest_page_i = -1;

    for (std::size_t i = 0; i < accesses.size(); i++) {
        int page = accesses[i];
        std::list<int>& page_uses = uses.at(page);

        page_uses.pop_front();
        int page_next_i = page_uses.front();

        int prev_furthest_page = furthest_page;
        if (page_next_i > furthest_page_i) {
            furthest_page_i = page_next_i;
            furthest_page = page;
        }

        // Página presente
        if (present_pages.count(page)){
            continue;
        }

        // Pagina ausente
        faults++;

        // Há molduras livres
        if (present_pages.size() < frame_amount) {
            present_pages.insert(page);
            // std::cout << std::endl;
            continue;
        }

        // Não há molduras livres
        present_pages.erase(prev_furthest_page);
        present_pages.insert(page);

        if (prev_furthest_page == furthest_page) {
            furthest_page_i = -1;
            for (int present_page : present_pages) {
                std::list<int>& present_uses = uses.at(present_page);
                int present_i = present_uses.front();
                if (present_i > furthest_page_i) {
                    furthest_page_i = present_i;
                    furthest_page = present_page;
                }
            }
        }
    }

    std::cout << "Ótimo: " << faults << " PFs" << std::endl;
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
    // Chamar algoritmo ótimo
    optimal(accesses);
}