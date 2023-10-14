
#include <vector>
#include <unordered_set>

#include "substitution_algorithm.h"

struct SimulationEntry {
    SubstitutionAlgorithm& algorithm;
    std::unordered_set<int> present_pages;
    unsigned int faults = 0;

    SimulationEntry(SubstitutionAlgorithm& algorithm);
};

class Simulator {
    int frame_amount;
    std::vector<SubstitutionAlgorithm*> algorithms;
public:
    Simulator(int frame_amount);

    void add_algorithm(SubstitutionAlgorithm& algorithm);

    void simulate();

    inline void access(SimulationEntry& entry, int page);
};