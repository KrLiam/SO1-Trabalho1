#pragma once

class Context {
    int registers[6];
    long int stackPointer;
    long int programCounter;
    long int status;
};