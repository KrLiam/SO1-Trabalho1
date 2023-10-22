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

void Simulator::optimal(std::vector<page_t>& accesses) {
    int max_i = accesses.size();

    std::unordered_map<page_t, std::list<int>> uses;
    for (int i = max_i - 1; i >= 0; i--) {
        page_t page = accesses[i];

        if (!uses.count(page)) {
            uses.emplace(page, std::list<int>({max_i}));
        }
        uses.at(page).push_front(i);
    }

    present_pages.clear();

    faults = 0;
    page_t furthest_page = -1;
    page_t furthest_page_i = -1;

    for (std::size_t i = 0; i < accesses.size(); i++) {
        page_t page = accesses[i];
        std::list<int>& page_uses = uses.at(page);

        page_uses.pop_front();
        page_t page_next_i = page_uses.front();

        page_t prev_furthest_page = furthest_page;
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
            for (page_t present_page : present_pages) {
                std::list<int>& present_uses = uses.at(present_page);
                page_t present_i = present_uses.front();
                if (present_i > furthest_page_i) {
                    furthest_page_i = present_i;
                    furthest_page = present_page;
                }
            }
        }
    }

    std::cout << "Ótimo: " << faults << " PFs" << std::endl;
}


void Simulator::simulate(std::vector<page_t>& accesses, bool run_optimal) {
    if (algorithms.size() > 0) {
        std::cout << frame_amount << " quadros" << std::endl
                << accesses.size() << " refs " << std::endl;
    }

    for (SubstitutionAlgorithm* algorithm : algorithms) {
        present_pages.clear();
        faults = 0;

        for (page_t page : accesses) {
            if (present_pages.count(page)) {
                // std::cout << "page hit " << page << std::endl;
                algorithm->accessed(page);
            }
            else {
                faults++;
                // std::cout << "page fault " << page;
                page_t page_to_remove = algorithm->replace(page);
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
    if (run_optimal) optimal(accesses);
}