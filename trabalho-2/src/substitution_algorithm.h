#pragma once

#include <vector>


class SubstitutionAlgorithm {
public:
    SubstitutionAlgorithm(int frame_amount);

    /**
     * Chamado quando uma página é acessada sem page fault.
    */
    virtual void access(int page);

    /**
     * Recebe o id da página que gerou o page fault.
     * Retorna a página que deve ser substituída.
    */ 
    virtual int replace(int page);

    void simulate(std::vector<int> page_accesses);
};