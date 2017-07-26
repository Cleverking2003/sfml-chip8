#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <random>
#include <ctime>
using namespace std;

class Chip8{
public:
    Chip8();
    void init(string fileName);
    void cycle();

    uint8_t vmem[64 * 32], key[16];
    bool drawFlag;

private:
    uint8_t v[16], dt, st, memory[4096];
    uint16_t i, pc, sp, stack[16];
    uint8_t* code;

    void incPC();
    void setF();
    void unsetF();
};
