#include "chip8.h"

u8 font[] = {
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
    0xf0, 0x80, 0xf0, 0x80, 0x80  //F
};

Chip8::Chip8(){}

void Chip8::init(string file){
    ifstream rom(file, ios::in | ios::binary);
    if(!rom.is_open()){
        printf("Error: file can't be opened\n");
    }

    rom.seekg(0, rom.end);
    int size = rom.tellg();
    rom.seekg(0, rom.beg);
    u8* code = new u8[size];
    rom.read((char*)code, size);

    for(auto& b : memory){
        b = 0;
    }

    for(int i = 0; i < size; i++){
        memory[512 + i] = code[i];
    }

    delete[] code;

    for(int i = 0; i < 80; i++){
        memory[i] = font[i];
    }

    for(auto& p : gfx){
        p = 0;
    }

    for(int i = 0; i < 16; i++){
        keys[i] = v[i] = stack[i] = 0;
    }

    I = sp = dt = st = 0;
    pc = 0x200;
    drawFlag = false;
    srand(time(NULL));
}

void Chip8::exec(){
    u16 opcode = (memory[pc] << 8) | memory[pc + 1];
    u8 x = (opcode >> 8) & 0xf, y = (opcode >> 4) & 0xf, kk = opcode & 0xff, n = opcode & 0xf;
    u16 nnn = opcode & 0xfff;

    switch(opcode >> 12){
    case 0:
        switch(kk){
        case 0xe0:
            printf("CLS\n");
            for(auto& p : gfx){
                p = 0;
            }
            pc += 2;
            break;
        case 0xee:
            printf("RET\n");
            pc = stack[sp];
            sp--;
            pc += 2;
            break;
        default:
            printf("Bad op\n");
            pc += 2;
            break;
        }
        break;
    case 1:
        printf("JP %04x\n", nnn);
        pc = nnn;
        break;
    case 2:
        printf("CALL %04x\n", nnn);
        sp++;
        stack[sp] = pc;
        pc = nnn;
        break;
    case 3:
        printf("SE V%01x, %02x\n", x, kk);
        if(v[x] == kk) pc += 2;
        pc += 2;
        break;
    case 4:
        printf("SNE V%01x, %02x\n", x, kk);
        if(v[x] != kk) pc += 2;
        pc += 2;
        break;
    case 5:
        printf("SE V%01x, V%01x\n", x, y);
        if(v[x] == v[y]) pc += 2;
        pc += 2;
        break;
    case 6:
        printf("LD V%01x, %02x\n", x, kk);
        v[x] = kk;
        pc += 2;
        break;
    case 7:
        printf("ADD V%01x, %02x\n", x, kk);
        v[x] += kk;
        pc += 2;
        break;
    case 8:
        switch(n){
        case 0:
            printf("LD V%01x, V%01x\n", x, y);
            v[x] = v[y];
            break;
        case 1:
            printf("OR V%01x, V%01x\n", x, y);
            v[x] |= v[y];
            break;
        case 2:
            printf("AND V%01x, V%01x\n", x, y);
            v[x] &= v[y];
            break;
        case 3:
            printf("XOR V%01x, V%01x\n", x, y);
            v[x] ^= v[y];
            break;
        case 4:
            printf("ADD V%01x, V%01x\n", x, y);
            if(v[x] + v[y] > 255) v[0xf] = 1;
            v[x] += v[y];
            break;
        case 5:
            printf("SUB V%01x, V%01x\n", x, y);
            if(v[x] > v[y]) v[0xf] = 1;
            v[x] -= v[y];
            break;
        case 6:
            printf("SHR V%01x\n", x);
            if(v[x] & 1) v[0xf] = 1;
            v[x] >>= 1;
            break;
        case 7:
            printf("SUBN V%01x, V%01x\n", x, y);
            if(v[y] > v[x]) v[0xf] = 1;
            v[x] = v[y] - v[x];
            break;
        case 0xe:
            printf("SHL V%01x\n", x);
            if(v[x] & 0x80) v[0xf] = 1;
            v[x] <<= 1;
            break;
        default:
            printf("Bad op\n");
            break;
        }
        pc += 2;
        break;
    case 9:
        printf("SNE V%01x, V%01x\n", x, y);
        if(v[x] != v[y]) pc += 2;
        pc += 2;
        break;
    case 0xa:
        printf("LD I, %04x\n", nnn);
        I = nnn;
        pc += 2;
        break;
    case 0xb:
        printf("JP V0, %04x\n", nnn);
        pc = v[0] + nnn;
        break;
    case 0xc:
        printf("RND V%01x, %02x\n", x, kk);
        v[x] = rand() % 256 & kk;
        pc += 2;
        break;
    case 0xd:{
        printf("DRW V%01x, V%01x, %01x\n", x, y, n);
        for (int yline = 0; yline < n; yline++)
        {
            u8 pixel = memory[I + yline];
            for(int xline = 0; xline < 8; xline++)
            {
                if((pixel & (0x80 >> xline)) != 0)
                {
                    if(gfx[(v[x] + xline + ((v[y] + yline) * 64))] == 1)
                    {
                        v[0xF] = 1;
                    }
                    gfx[v[x] + xline + ((v[y] + yline) * 64)] ^= 1;
                }
            }
        }
        }
        drawFlag = true;
        pc += 2;
        break;
    case 0xe:
        switch(kk){
        case 0x9e:
            printf("SKP V%01x\n", x);
            if(keys[v[x]]) pc += 2;
            break;
        case 0xa1:
            printf("SKNP V%01x\n", x);
            if(!keys[v[x]]) pc += 2;
            break;
        default:
            printf("Bad op\n");
            break;
        }
        pc += 2;
        break;
    case 0xf:
        switch(kk){
        case 7:
            printf("LD V%01x, DT\n", x);
            v[x] = dt;
            break;
        case 0xa:{
            printf("LD V%01x, K\n", x);
            bool pressed;
            for(int i = 0; i < 16; i++){
                if(keys[i]){
                    pressed = true;
                    v[x] = i;
                }
            }
            if(!pressed) return;
        }
        break;
        case 0x15:
            printf("LD DT, V%01x\n", x);
            dt = v[x];
            break;
        case 0x18:
            printf("LD ST, V%01x\n", x);
            st = v[x];
            break;
        case 0x1e:
            printf("ADD I, V%01x\n", x);
            I += v[x];
            break;
        case 0x29:
            printf("LD F, V%01x\n", x);
            I = 5 * v[x];
            break;
        case 0x33:
            printf("LD B, V%01x\n", x);
            memory[I] = v[x] / 100;
            memory[I + 1] = v[x] / 10;
            memory[I + 2] = v[x] % 10;
            break;
        case 0x55:
            printf("LD [I], V%01x\n", x);
            for(int i = 0; i <= x; i++){
                v[i] = memory[I + i];
            }
            break;
        case 0x65:
            printf("LD V%01x, [I]\n", x);
            for(int i = 0; i <= x; i++){
                v[i] = memory[I + i];
            }
            break;
        default:
            printf("Bad op\n");
            break;
        }
        pc += 2;
        break;
    default:
        printf("Bad op\n");
        pc += 2;
        break;
    }

    if(dt > 0) dt--;
    if(st > 0) st--;
}
