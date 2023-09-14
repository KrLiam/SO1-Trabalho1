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
    ContextINE5412();

    ContextINE5412(ContextINE5412& context);

    void show();

    void tick(int seed);

    constexpr ContextINE5412& operator=(const ContextINE5412& context) {
        for (int i = 0; i < size; i++) {
            registers[i] = context.registers[i];
        }
        stackPointer = context.stackPointer;
        programCounter = context.programCounter;
        status = context.status;
        return *this;
    }
};

class ContextMIPS : public Context {
    static const int size = 4;
    long int registers[size];

public:
    ContextMIPS();

    ContextMIPS(ContextMIPS& context);

    void show();

    void tick(int seed);

    constexpr ContextMIPS& operator=(const ContextMIPS& context) {
        for (int i = 0; i < size; i++) {
            registers[i] = context.registers[i];
        }
        return *this;
    }
};
