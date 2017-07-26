#include "chip8.h"

uint8_t font[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xf0, 0x10, 0xf0, 0x80, 0xf0, //2
    0xf0, 0x10, 0xf0, 0x10, 0xf0, //3
    0x90, 0x90, 0xf0, 0x10, 0x10, //4
    0xf0, 0x80, 0xf0, 0x10, 0xf0, //5
    0xf0, 0x80, 0xf0, 0x90, 0xf0, //6
    0xf0, 0x10, 0x20, 0x40, 0x40, //7
    0xf0, 0x90, 0xf0, 0x90, 0xf0, //8
    0xf0, 0x90, 0xf0, 0x10, 0xf0, //9
    0xf0, 0x90, 0xf0, 0x90, 0x90, //A
    0xe0, 0x90, 0xe0, 0x90, 0xe0, //B
    0xf0, 0x80, 0x80, 0x80, 0xf0, //C
    0xe0, 0x90, 0x90, 0x90, 0xe0, //D
    0xf0, 0x80, 0xf0, 0x80, 0xf0, //E
    0xf0, 0x80, 0xf0, 0x80, 0x80 //F
};

Chip8::Chip8(){} //Nothing to do here

//Init chip 8
void Chip8::init(string fileName){
    i = 0;
    sp = 15;
    pc = 0x200;
    dt = st = 0;
    srand(time(NULL));

    for(uint8_t c : vmem){
        c = 0;
    }

    for(int j = 0; j < 16; j++){
        key[j] = 0;
        stack[j] = 0;
        v[j] = 0;
    }

    for(uint8_t c : memory){
        c = 0;
    }

    ifstream rom(fileName.c_str(), ios::in | ios::binary);
    rom.seekg(0, rom.end);
    int size = rom.tellg();
    rom.seekg(0, rom.beg);
    rom.read((char*)code, size);
    rom.close();

    for(int j = 0; j < 80; j++) memory[j] = font[j];

    for(int j = 0; j < size; j++){
        memory[j + 0x200] = code[j];
    }
}

void Chip8::cycle(){
    //Get opcode and some values
    uint16_t opcode = memory[pc];
    uint8_t x = (opcode >> 8) & 0xf, y = (opcode >> 4) & 0xf, kk = opcode >> 8;
    uint16_t nnn = opcode & 0xfff;

    //Switch all of these
    switch(opcode >> 12){
    case 0:
        if(opcode == 0x00e0){
            for(uint8_t p : vmem) p = 0;
        }

        else if(opcode == 0x00ee){
            pc = stack[sp];
            sp--;
        }
        break;
    case 1:
        pc = nnn;
        break;
    case 2:
        sp++;
        stack[sp] = pc;
        pc = nnn;
        break;
    case 3:
        if(v[x] == kk) incPC();
        break;
    case 4:
        if(v[x] != kk) incPC();
        break;
    case 5:
        if(v[x] == v[y]) incPC();
        break;
    case 6:
        v[x] = kk;
        incPC();
        break;
    case 7:
        v[x] += kk;
        incPC();
        break;
    case 8:
        switch(opcode & 0xf){
        case 0:
            v[x] = v[y];
            incPC();
            break;
        case 1:
            v[x] |= v[y];
            incPC();
            break;
        case 2:
            v[x] &= v[y];
            incPC();
            break;
        case 3:
            v[x] ^= v[y];
            incPC();
            break;
        case 4:
            if(v[x] + v[y] > 255) setF();
            v[x] += v[y];
            incPC();
            break;
        case 5:
            if(v[y] < v[x]) setF();
            v[x] -= v[y];
            incPC();
            break;
        case 6:
            if(v[x] & 1 == 1) setF();
            v[x] >>= 1;
            incPC();
            break;
        case 7:
            if(v[y] > v[x]) setF();
            v[x] = v[y] - v[x];
            incPC();
            break;
        case 0xe:
            if((v[x] >> 7) & 1 == 1) setF();
            v[x] <<= 1;
            incPC();
            break;
        }
        break;
    case 9:
        if(v[x] != v[y]) incPC();
        break;
    case 0xa:
        i = nnn;
        incPC();
        break;
    case 0xb:
        pc = nnn + v[0];
        incPC();
        break;
    case 0xc:
        v[x] = rand() & 0xff & kk;
        incPC();
        break;
    case 0xd:
        {
            drawFlag = true;
            uint8_t n = opcode & 0xf;
            uint8_t sprite[n];
            for(int j = 0; j < n; j++) sprite[j] = memory[i + j];
            for(int sy = v[y]; sy < n; sy++){
                for(int sx = v[x]; sx < 8; sx++){
                    uint8_t pixel = memory[sy] >> sx & 1;
                    if(vmem[sy * 64 + sx] ^= pixel == 0) setF();
                    vmem[sy * 64 + sx] ^= pixel;
                }
            }
            incPC();
        }

        break;
    case 0xe:
        if(opcode & 0xff == 0x9e){
            if(key[v[x]] == 1) incPC();
        }
        else if(opcode & 0xff == 0xa1){
            if(key[v[x]] == 0) incPC();
        }
        break;
    case 0xf:
        switch(opcode & 0xff){
        case 7:
            v[x] = dt;
            incPC();
            break;
        case 0xa: //Compiler doesn't like new vars in cases, use {} block
            {bool press = false;
            for(uint8_t k : key){
                if(k == 1) press = true;
            }

            if(!press){
                return;
            }
            else{
                incPC();
            }
            }
            break;
        case 0x15:
            dt = v[x];
            incPC();
            break;
        case 0x18:
            st = v[x];
            incPC();
            break;
        case 0x1e:
            i += v[x];
            incPC();
            break;
        case 0x29:
            {uint16_t sprPos = v[x] * 5; //size of font is 5 bytes
            i = sprPos;
            incPC();}
            break;
        case 0x33:
            memory[i] = v[x] / 100;
            memory[i + 1] = v[x] / 10;
            memory[i + 2] = v[x] % 10;
            incPC();
            break;
        case 0x55:
            for(int j = 0; j < 16; j++){
                memory[i + j] = v[j];
            }
            incPC();
            break;
        case 0x65:
            for(int j = 0; j < 16; j++){
                v[j] = memory[i + j];
            }
            incPC();
            break;
        }
    }
}

void Chip8::incPC(){
    pc += 2;
}

//Set and unset VF
void Chip8::setF(){
    v[15] = 1;
}

void Chip8::unsetF(){
    v[15] = 0;
}
