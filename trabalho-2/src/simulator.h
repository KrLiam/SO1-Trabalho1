
#include <vector>


struct PageEntry {
    unsigned int address;
    bool present;
};

class Simulator {
    std::vector<PageEntry> page_table;
public:
    Simulator();

    void simulate(const std::vector<int>& page_accesses);
};