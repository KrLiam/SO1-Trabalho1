#pragma once

class Context {
    public:
        virtual ~Context() {};
        virtual void show() = 0;
        virtual void tick(int seed) = 0;
};

class ContextINE5412 : public Context {
    int size = 9;
    long int registers[6];
    long int stackPointer;
    long int programCounter;
    long int status;

public:
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
        std::cout << "R=[";
        for (int i = 0; i < 5; i++) {
            std::cout << registers[i] << ", ";
        }
        std::cout << registers[6] << "]"
                  << ", SP=" << stackPointer
                  << ", PC=" << programCounter
                  << ", S="  << status << std::endl;
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
    static const int size = 4;
    long int registers[size];

public:
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
        for (int i = 0; i < size; i++) {
            registers[i] = 0;
        }
    }

    ContextMIPS(ContextMIPS& context) {
        for (int i = 0; i < size; i++) {
            registers[i] = context.registers[i];
        }
    }
};