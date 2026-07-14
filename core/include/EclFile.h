#pragma once
#include "Chireiden.h"
#include "EclInstruction.h"
#include "Macros.h"

struct EclFileHeader
{
    uint32_t magic; // 0x0
    uint16_t version; // 0x4
    uint16_t includeLength; // 0x6 /* include_offset + ANIM+ECLI length */
    uint32_t includeOffset; // 0x8
    int __dword_C; // 0xC
    uint16_t numSubroutines; // 0x10
    
    int __dword_14; // 0x14
    int __dword_18; // 0x18
    int __dword_1C; // 0x1C
    int __dword_20; // 0x20
};

struct EclFile
{
    EclFileHeader header;
    LPBYTE bytes;
};

struct EclSubroutineRef
{
    char* name;          // 0x0
    uint8_t* bytecode;   // 0x4
};
ASSERT_SIZE(EclSubroutineRef, 0x8);

struct EclSubroutineHeader
{
    uint32_t magic;
    uint32_t dataOffset;
    int idk;
    int dummy[2];
    uint8_t instructions[]; // Actually EclInstr[]
};

struct EclSubroutine
{
    const char* name;
    EclSubroutineHeader header;
};

struct EclInclude
{
    uint32_t magic; // 0x0
    uint32_t count; // 0x4
    char names[2]; // 0x8 probably variable length
};