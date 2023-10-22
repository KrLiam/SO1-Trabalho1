
#include <vector>
#include <unordered_set>

#include "substitution_algorithm.h"


class Simulator {
    unsigned int frame_amount;
    std::vector<SubstitutionAlgorithm*> algorithms;
    std::unordered_set<page_t> present_pages;
    unsigned int faults = 0;
public:
    Simulator(unsigned int frame_amount);

    void add_algorithm(SubstitutionAlgorithm& algorithm);

    void simulate(std::vector<page_t>& accesses, bool run_optimal);

    void optimal(std::vector<page_t>& accesses);
};