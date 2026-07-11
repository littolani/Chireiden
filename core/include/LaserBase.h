#pragma once
#include "Chireiden.h"
#include "LaserSegment.h"
#include "Timer.h"
#include "Macros.h"
#include "Vectors.h"

struct LaserBaseVftable
{
    void* vfunction1;
    void* vfunction2;
    void* vfunction3;
    void* vfunction4;
    void* vfunction5;
    void* vfunction6;
    void* vfunction7;
    void* vfunction8;
    void* vfunction9;
    void* vfunction10;
    void* vfunction11;
    void* vfunction12;
    void* vfunction13;
    void* vfunction14;
    void* vfunction15;
    void* vfunction16;
    void* vfunction17;
    void* vfunction18;
    void* vfunction19;
    void* vfunction20;
};

struct LaserTaskData
{
    Timer timer;            // 0x00 - 0x14
    float float_0x14;       // 0x14
    float float_0x18;       // 0x18
    float float_0x1c;       // 0x1c
    float float_0x20;       // 0x20
    int int_0x24;           // 0x24
    int int_0x28;           // 0x28
    int int_0x2c;           // 0x2c
    int int_0x30;           // 0x30
};
ASSERT_SIZE(LaserTaskData, 0x34);

class LaserBase
{
public:
    LaserBaseVftable* laserDataVtable_0x0;        // 0x00
    uint32_t laserDataPtr_0x4;        // 0x04
    void* laserLineVtable_0x8;        // 0x08
    uint32_t opcode;                  // 0x0c
    uint32_t u1_0x10;                 // 0x10
    Timer timer_0x14;                 // 0x14
    Timer timer_0x28;                 // 0x28
    Float3 laserBasePosition;         // 0x3c
    Float3 vec_0x48;                  // 0x48
    float laserAngle;                 // 0x54
    float laserLength;                // 0x58
    float laserWidth;                 // 0x5c
    float currentTime_0x60;           // 0x60
    float someSmallFloat_0x64;        // 0x64
    int idk0;                         // 0x68
    int someLimit_0x6c;               // 0x6c
    LaserTaskData tasks[18];          // 0x70
    int instructionIndex;             // 0x418
    uint32_t vfunc_dispatch_opcode;   // 0x41c
    int idk14;                        // 0x420
    Timer timer1;                     // 0x424
    int bulletType;                   // 0x438
    short short_0x43c;                // 0x43c
    short scriptNumber;              // 0x43e

    LaserBase();
};
ASSERT_SIZE(LaserBase, 0x440);