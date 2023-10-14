#pragma once

#include <vector>
#include <queue>
#include <list>
#include <algorithm>


class SubstitutionAlgorithm {
protected:
    unsigned int frame_amount;
public:

    virtual ~SubstitutionAlgorithm() {};

    void set_frame_amount(unsigned int frame_amount) {
        this->frame_amount = frame_amount;
    }

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


class LRU : public SubstitutionAlgorithm {
    std::list<int> list;
public:

    const char* name() { return "LRU"; }

    void accessed(int page) {
        auto it = std::find(list.begin(), list.end(), page);
        list.erase(it);
        list.push_front(page);
    }

    int replace(int page) {
        list.push_front(page);
        
        if (list.size() <= frame_amount) return -1;

        int page_to_remove = list.back();
        list.pop_back();
        return page_to_remove;
    }
};