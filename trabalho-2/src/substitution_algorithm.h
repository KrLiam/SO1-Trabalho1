#pragma once

#include <vector>
#include <queue>


class SubstitutionAlgorithm {
protected:
    unsigned int frame_amount;
public:

    SubstitutionAlgorithm(unsigned int frame_amount) : frame_amount(frame_amount) {}

    virtual const char* name() = 0;

    /**
     * Chamado quando uma página é acessada sem page fault.
    */
    virtual void accessed(int) {};

    /**
     * Recebe o id da página que gerou o page fault.
     * Retorna a página que deve ser substituída.
    */ 
    virtual int replace(int page) = 0;

    void simulate(std::vector<int>) {};
};


class FIFO : public SubstitutionAlgorithm {
    std::queue<int> queue;
public:

    FIFO(unsigned int frame_amount) : SubstitutionAlgorithm(frame_amount) {}

    const char* name() { return "FIFO"; }

    virtual int replace(int page) {
        queue.push(page);

        // se ainda há molduras livres, não retira nenhuma página
        if (queue.size() <= frame_amount) return -1;

        int page_to_remove = queue.front();
        queue.pop();
        return page_to_remove;
    }
};