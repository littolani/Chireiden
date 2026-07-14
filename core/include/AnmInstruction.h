#pragma once
#include <cstdint>

struct AnmRawInstruction
{
    int16_t opcode;             // 0x0
    int16_t offsetToNextInstr;  // 0x2
    short time;                 // 0x4
    uint16_t varMask;           // 0x6
    int args[10];               // 0x8 - 0x2C
};