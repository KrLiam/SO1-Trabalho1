#pragma once

#include <vector>
#include <queue>
#include <list>
#include <algorithm>

typedef int page_t;

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
    virtual void accessed(page_t) {};

    /**
     * Retorna o id da página a ser removida numa substituição.
    */
    virtual page_t remove() = 0;

    /**
     * Recebe o id de uma página a ser inserida após um page fault. 
    */
    virtual void insert(page_t page) = 0;
    

    void simulate(std::vector<page_t>) {};
};


class FIFO : public SubstitutionAlgorithm {
    std::queue<page_t> queue;
public:

    const char* name() { return "FIFO"; }

    virtual void insert(page_t page) {
        queue.push(page);
    }

    virtual page_t remove() {
        page_t page_to_remove = queue.front();
        queue.pop();
        return page_to_remove;
    }
};


class LRU : public SubstitutionAlgorithm {
    std::list<page_t> list;
public:

    const char* name() { return "LRU"; }

    void accessed(page_t page) {
        auto it = std::find(list.begin(), list.end(), page);
        list.erase(it);
        list.push_front(page);
    }

    virtual void insert(page_t page) {
        list.push_front(page);   
    }

    virtual page_t remove() {
        page_t page_to_remove = list.back();
        list.pop_back();
        return page_to_remove;
    }
};