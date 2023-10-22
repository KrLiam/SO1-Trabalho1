
#include <vector>
#include <unordered_set>

#include "substitution_algorithm.h"

struct SimulationEntry {
    const std::string name;
    unsigned int faults;

    SimulationEntry(const std::string name, unsigned int faults);
};

struct SimulationResult {
    std::vector<SimulationEntry> entries;
    const std::vector<page_t>& accesses;
    unsigned int frame_amount;

    SimulationResult(
        std::vector<SimulationEntry> entries,
        const std::vector<page_t>& accesses,
        unsigned int frame_amount
    );
};

class Simulator {
    unsigned int frame_amount;
    std::vector<SubstitutionAlgorithm*> algorithms;
    std::unordered_set<page_t> present_pages;
public:
    Simulator(unsigned int frame_amount);

    void add_algorithm(SubstitutionAlgorithm& algorithm);

    SimulationResult simulate(std::vector<page_t>& accesses, bool run_optimal);

    unsigned int optimal(std::vector<page_t>& accesses);
};

void print_simulation_result(const SimulationResult& result);