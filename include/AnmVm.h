#pragma once
#include "Chireiden.h"
#include "AnmLoaded.h"
#include "Timer.h"
#include "Interp.h"

struct Float3
{
    float x, y, z;
};

struct Float2
{
    float x, y;
};

struct Int3 
{
    int x, y, z;
};

class AnmVm;
struct AnmVmList
{
    AnmVm* entry;
    AnmVmList* next;
    AnmVmList* prev;
};

struct AnmRawInstruction
{
    uint16_t opcode;
    uint16_t offsetToNextInstr;
    short time;
    uint16_t varMask;
    uint32_t args[10];
};

class AnmVm
{
public:
    int id;
    AnmVmList nodeInGlobalList;
    AnmVmList nodeAsFamilyMember;
    AnmVm* nextInLayerList;
    int layer;
    D3DXVECTOR3 rotation;
    D3DXVECTOR3 angularVelocity;
    D3DXVECTOR2 scale;
    D3DXVECTOR2 scaleGrowth;
    D3DXVECTOR2 spriteSize;
    D3DXVECTOR2 uvScrollPos;
    Timer timeInScript; // <0x5c>
    D3DXVECTOR2 spriteUvQuad[4];
    Interp<Float3> posInterp;
    Interp<Int3> rgbInterp;
    Interp<int> alphaInterp;
    Interp<Float3> rotationInterp;
    Interp<Float2> scaleInterp;
    Interp<Int3> rgb2Interp;
    Interp<int> alpha2Interp;
    Interp<float> uVelInterp;
    Interp<float> vVelInterp;
    D3DXVECTOR2 uvScrollVel;
    D3DXMATRIX matrix2fc;
    D3DXMATRIX matrix33c;
    D3DXMATRIX matrix37c;
    D3DCOLOR color1;
    D3DCOLOR color2;
    uint16_t pendingInterrupt;
    uint16_t unused;
    Timer interruptReturnTime;
    AnmRawInstruction* interruptReturnInstr;
    int timeOfLastSpriteSet;
    uint16_t spriteNumber;
    uint16_t anmFileIndex;
    uint16_t anotherSpriteNumber;
    uint16_t scriptNumber;
    AnmRawInstruction* beginningOfScript;
    AnmRawInstruction* currentInstruction;
    AnmLoadedSprite* sprite;
    AnmLoaded* anmLoaded;
    int intVars[4]; // Script variables
    float floatVars[4]; // Script variables
    int intVar8;
    int intVar9;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 entityPos;
    D3DXVECTOR3 pos2;
    void* specialRenderData;
    uint32_t flagsLow;
    uint32_t flagsHigh;
    uint32_t unknown;
    void* onTick; // Function pointers to chainCallback
    void* onDraw; // Function pointers to chainCallback
    uint32_t unknown1;
    uint8_t fontDimensions[2];
    uint16_t probablyPadding;
    uint32_t j5;
    uint32_t j6;
    void* unknownFunc;
    void* spriteMappingFunc;
    void* dataForSpriteMappingFunc;

    void initialize();
    void run();

    // Placeholder functions
    int getIntVar(int index) { return 0; }
    float getFloatVar(int index) { return 0.0f; }
    int* getIntVarPtr(int index) { return nullptr; }
    float* getFloatVarPtr(int index) { return nullptr; }
    int randInt(int min, int max) { return min; } // Placeholder
    float randFloat(float min, float max) { return min; } // Placeholder
};
ASSERT_SIZE(AnmVm, 0x434);