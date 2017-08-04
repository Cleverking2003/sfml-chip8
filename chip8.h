#pragma once
#include <string>
#include <fstream>
#include <cstdio>
#include <random>
#include <ctime>
#include <cstdint>
using namespace std;
typedef uint8_t u8;
typedef uint16_t u16;

class Chip8{
public:
    u8 keys[16], gfx[64 * 32], dt, st;
    bool drawFlag;

    Chip8();
    void init(string file);
    void exec();

private:
    u8 v[16], memory[4096];
    u16 I, pc, sp, stack[16];
};
