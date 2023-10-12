
#include <vector>
#include <set>

#include "substitution_algorithm.h"

struct SimulationEntry {
    SubstitutionAlgorithm& algorithm;
    std::set<int> present_pages;

    SimulationEntry(SubstitutionAlgorithm& algorithm);
};

class Simulator {
    int frame_amount;
    std::set<int> present_pages;
    std::vector<SubstitutionAlgorithm*> algorithms;
public:
    Simulator(int frame_amount);

    void simulate();

    inline void access(SimulationEntry& entry, int page);
};