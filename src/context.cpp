#include <iostream>
#include "context.h"

// INE5412
ContextINE5412::ContextINE5412() {
    for (int i = 0; i < 6; i++) {
        registers[i] = 0;
    }
    stackPointer = 0;
    programCounter = 0;
    status = 0;
}

ContextINE5412::ContextINE5412(ContextINE5412& context) {
    for (int i = 0; i < 6; i++) {
        registers[i] = context.registers[i];
    }
    stackPointer = context.stackPointer;
    programCounter = context.programCounter;
    status = context.status;
}

void ContextINE5412::show() {
    std::cout << "R=[";
    for (int i = 0; i < 5; i++) {
        std::cout << registers[i] << ", ";
    }
    std::cout << registers[6] << "]"
                << ", SP=" << stackPointer
                << ", PC=" << programCounter
                << ", S="  << status << std::endl;
}

void ContextINE5412::tick(int seed) {
    srand(seed);
    programCounter++;
    for (int i = 0; i < 6; i++) {
        if (registers[i] != 0) {
            registers[i] += rand() % 100;
        } else {
            registers[i]++;
        }
    }
}



// MIPS
void ContextMIPS::show() {
    for (int i = 0; i < size-1; i++) {
        std::cout << registers[i] << ", ";
    }
    std::cout << registers[size-1] << std::endl;
}

void ContextMIPS::tick(int seed) {
    srand(seed+10);
    for (int i = 0; i < size; i++) {
            if (registers[i] == 0) {
            registers[i] += rand() % 100;
        } else {
            registers[i]++;
        }
        }
}

ContextMIPS::ContextMIPS() {
    for (int i = 0; i < size; i++) {
        registers[i] = 0;
    }
}

ContextMIPS::ContextMIPS(ContextMIPS& context) {
    for (int i = 0; i < size; i++) {
        registers[i] = context.registers[i];
    }
}
