#include <iostream>
#include <unordered_map>
#include <set>
#include <list>

#include "simulator.h"
#include "substitution_algorithm.h"


SimulationEntry::SimulationEntry(const std::string name, unsigned int faults)
    : name(name),
      faults(faults) {}

SimulationResult::SimulationResult(
    std::vector<SimulationEntry> entries,
    const std::vector<page_t>& accesses,
    unsigned int frame_amount
) : entries(entries),
    accesses(accesses),
    frame_amount(frame_amount) {}

Simulator::Simulator(unsigned int frame_amount) : frame_amount(frame_amount) {}

void Simulator::add_algorithm(SubstitutionAlgorithm& algorithm) {
    algorithm.set_frame_amount(frame_amount);
    algorithms.push_back(&algorithm);
}

unsigned int Simulator::optimal(std::vector<page_t>& accesses) {
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

    unsigned int faults = 0;
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

    return faults;
}


SimulationResult Simulator::simulate(std::vector<page_t>& accesses, bool run_optimal) {
    std::vector<SimulationEntry> result_entries;

    for (SubstitutionAlgorithm* algorithm : algorithms) {
        present_pages.clear();
        unsigned int faults = 0;

        for (page_t page : accesses) {
            if (present_pages.count(page)) {
                // std::cout << "page hit " << page << std::endl;
                algorithm->accessed(page);
            }
            else {
                faults++;
                // std::cout << "page fault " << page;

                if (present_pages.size() >= frame_amount) {
                    page_t removed_page = algorithm->remove();
                    present_pages.erase(removed_page);
                    // std::cout << " removed page " << removed_page;
                }
                // std::cout << std::endl;

                algorithm->insert(page);
                present_pages.insert(page);
            }
        }

        result_entries.emplace_back(algorithm->name(), faults);
    
    }
    // Chamar algoritmo ótimo
    if (run_optimal) {
        unsigned int optimal_faults = optimal(accesses);
        result_entries.emplace_back("OPT", optimal_faults);
    }

    return SimulationResult(result_entries, accesses, frame_amount);
}

void print_simulation_result(const SimulationResult& simulation) {
    std::cout << simulation.frame_amount << " quadros" << std::endl
              << simulation.accesses.size() << " refs " << std::endl;

    for (const SimulationEntry& entry : simulation.entries) {
        std::cout << entry.name << ": " << entry.faults << " PFs" << std::endl;
    }
}