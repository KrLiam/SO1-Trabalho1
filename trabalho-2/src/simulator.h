
#include <vector>
#include <unordered_set>

#include "substitution_algorithm.h"


class Simulator {
    int frame_amount;
    std::vector<SubstitutionAlgorithm*> algorithms;
    std::unordered_set<int> present_pages;
    unsigned int faults = 0;
public:
    Simulator(int frame_amount);

    void add_algorithm(SubstitutionAlgorithm& algorithm);

    void simulate(std::vector<int>& accesses);

    void optimal(std::vector<int>& accesses);
};