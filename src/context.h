#pragma once

class Context {
    public:
        void show() {
            std::cout << "WTF HOWD U GET HERE" << std::endl;
        };
        void tick(int seed) {};
};

class ContextINE5412 : public Context {
public:
    int size = 9;
    long int registers[6];
    long int stackPointer;
    long int programCounter;
    long int status;


    ContextINE5412() {
        for (int i = 0; i < 6; i++) {
            registers[i] = 0;
        }
        stackPointer = 0;
        programCounter = 0;
        status = 0;
    }

    ContextINE5412(ContextINE5412& context) {
        for (int i = 0; i < 6; i++) {
            registers[i] = context.registers[i];
        }
        stackPointer = context.stackPointer;
        programCounter = context.programCounter;
        status = context.status;
    }

    void show() {
        std::cout << "registers: ";
        for (int i = 0; i < 6; i++) {
            std::cout << registers[i] << ", ";
        }
        std::cout << "stackPointer: " << stackPointer;
        std::cout << ", programCounter: " << programCounter;
        std::cout << ", status: " << status << std::endl;
    }

    void tick(int seed) {
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
};

class ContextMIPS : public Context {
public:
    int size = 5;
    long int registers[5];
    void show() {
        for (int i = 0; i < size-1; i++) {
            std::cout << registers[i] << ", ";
        }
        std::cout << registers[size-1] << std::endl;
     }

    void tick(int seed) {
        srand(seed+10);
        for (int i = 0; i < size; i++) {
                if (registers[i] == 0) {
                registers[i] += rand() % 100;
            } else {
                registers[i]++;
            }
            }
    }

    ContextMIPS() {
        for (int i = 0; i < 6; i++) {
            registers[i] = 0;
        }
    }

    ContextMIPS(ContextMIPS& context) {
        for (int i = 0; i < 32; i++) {
            registers[i] = context.registers[i];
        }
    }
};