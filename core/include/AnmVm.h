#pragma once
#include "AnmId.h"
#include "AnmInstruction.h"
#include "AnmVmList.h"
#include "RenderVertex.h"
#include "Timer.h"
#include "Globals.h"
#include "Interp.h"
#include "DistortionMesh.h"
#include "Supervisor.h"
#include "Chain.h"
#include "Vectors.h"
#include "Rng.h"
#include <bit>
#include <type_traits>

struct AnmLoaded;
struct DistortionMesh;
struct AnmLoadedSprite;

struct SpecialRenderData
{
    RenderVertex144 vertices[33]; // 0x000 - 0x39C
    float padding;                // Alignment padding likely
    float currentRadii[32];       // 0x3A0
    float radiusDeltas[32];       // 0x420
    float uvScrollSpeedU;         // 0x4A4
    float uvScrollSpeedV;         // 0x4A8
};

class AnmVm
{
public:
    AnmId m_id;                                              // <0x0>
    AnmVmListNode m_globalListNode;                          // <0x4>
    AnmVmListNode m_familyListNode;                          // <0x10>
    AnmVm* m_nextInLayerList;                                // <0x1c>
    int m_layer;                                             // <0x20>
    Float3 m_rotation;                                       // <0x24>
    Float3 m_angularVelocity;                                // <0x30>
    Float2 m_scale;                                          // <0x3c>
    Float2 m_scaleGrowth;                                    // <0x44>
    Float2 m_spriteSize;                                     // <0x4c>
    Float2 m_uvScrollPos;                                    // <0x54>
    Timer m_timeInScript;                                    // <0x5c>
    Float2 m_spriteUvQuad[4];                                // <0x70>
    Interp<Float3> m_posInterp;                              // <0x90>
    Interp<Int3> m_rgbInterp;                                // <0xdc>
    Interp<int> m_alphaInterp;                               // <0x128>
    Interp<Float3> m_rotationInterp;                         // <0x154>
    Interp<Float2> m_scaleInterp;                            // <0x1a0>
    Interp<Int3> m_rgb2Interp;                               // <0x1dc>
    Interp<int> m_alpha2Interp;                              // <0x228>
    Interp<float> m_uVelInterp;                              // <0x254>
    Interp<float> m_vVelInterp;                              // <0x280>
    Float2 m_uvScrollVel;                                    // <0x2ac>
    D3DXMATRIX m_baseScaleMatrix;                            // <0x2b4>
    D3DXMATRIX m_localTransformMatrix;                       // <0x2f4>
    D3DXMATRIX m_textureMatrix;                              // <0x334>
    D3DCOLOR m_color0;                                       // <0x374>
    D3DCOLOR m_color1;                                       // <0x378>
    uint16_t m_pendingInterrupt;                             // <0x37c>
    uint16_t m_unused;                                       // <0x37e>
    Timer m_interruptReturnTime;                             // <0x380>
    AnmRawInstruction* m_interruptReturnInstr;               // <0x394>
    int m_timeOfLastSpriteSet;                               // <0x398>
    uint16_t m_spriteNumber;                                 // <0x39c>
    uint16_t m_anmFileIndex;                                 // <0x39e>
    uint16_t m_anotherSpriteNumber;                          // <0x3a0>
    uint16_t m_scriptNumber;                                 // <0x3a2>
    AnmRawInstruction* m_beginningOfScript;                  // <0x3a4>
    AnmRawInstruction* m_currentInstruction;                 // <0x3a8>
    AnmLoadedSprite* m_sprite;                               // <0x3ac>
    AnmLoaded* m_anmLoaded;                                  // <0x3b0>
    int m_intVars[4];                                        // <0x3b4>
    float m_floatVars[4];                                    // <0x3c4>
    int m_intVar8;                                           // <0x3d4>
    int m_intVar9;                                           // <0x3d8>
    Float3 m_pos;                                            // <0x3dc>
    Float3 m_entityPos;                                      // <0x3e8>
    Float3 m_offsetPos;                                      // <0x3f4>
    SpecialRenderData* m_specialRenderData;                  // <0x400>
    uint32_t m_flagsLow;                                     // <0x404>
    uint32_t m_flagsHigh;                                    // <0x408>
    uint32_t m_unknown;                                      // <0x40c>
    ChainCallback m_onTick;                                  // <0x410>
    ChainCallback m_onDraw;                                  // <0x414>
    DistortionMesh* m_distortionMesh;                        // <0x418>
    uint8_t m_fontDimensions[2];                             // <0x41c>
    uint16_t m_probablyPadding;                              // <0x41e>
    uint32_t m_j5;                                           // <0x420>
    uint32_t m_j6;                                           // <0x424>
    void* m_unknownFunc;                                     // <0x428>
    void* (*m_spriteMappingFunc)(AnmVm* vm, uint32_t param); // <0x42c>
    void* m_dataForSpriteMappingFunc;                        // <0x430>

    AnmVm();
    ~AnmVm();
    static void initialize(AnmVm* This);
    static void run(AnmVm* This);

    /**
     * 0x4503d0
     * @brief Used for a spinning animations such as the rocks in stage 1 and "SUBTERRANEAN ANIMISM" title screen text (identical to writeSpriteCharacters)
     * @param  This        ECX:4
     * @param  bottomLeft  Stack[0x4]:4
     * @param  bottomRight EBX:4
     * @param  topLeft     EDI:4
     * @param  topRight    ESI:4
     */
    static void applyZRotationToQuadCorners(
        AnmVm* This,
        Float3* bottomLeft,
        Float3* bottomRight,
        Float3* topLeft,
        Float3* topRight
    );

    /**
     * 0x4503d0
     * @brief Used for the Tou Hou Chi Rei Den characters on the title screen (identical to applyZRotationToQuadCorners)
     * @param This        Stack[0x4]:4
     * @param bottomLeft  EBX:4
     * @param bottomRight EDI:4
     * @param topRight    ESI:4
     * @param topLeft     EDX:4
     */
    static void writeSpriteCharacters(
        AnmVm* This,
        Float3* bottomLeft,
        Float3* bottomRight,
        Float3* topRight,
        Float3* topLeft
    );

    static void writeSpriteCharactersWithoutRot(
        AnmVm* This,
        Float3* bottomLeft,
        Float3* bottomRight,
        Float3* topRight,
        Float3* topLeft
    );

    //static void onDrawRenderMesh();
    static int setupTextureQuadAndMatrices(AnmVm* This, uint32_t spriteNumber, AnmLoaded* anmLoaded);

    /**
     * 0x450700
     * @brief Used for the pillar background in the Extra stage
     * @param  This EAX:4
     * @return int  EAX:4
     */
    static int projectQuadCornersThroughCameraViewport(AnmVm* This);

    static float wrapAngleSum(float f1, float f2);
    static float randScale(float f, RngContext* ctx);
    static void inlineAsmFsincos(RenderVertex144* This, float theta, float scale);
    static int onTick(AnmVm* This);
    static int getIntVar(AnmVm* This, int id);
    static int* getIntVarPtr(AnmVm* This, int* id);
    static float getFloatVar(AnmVm* This, float id);
    static float* getFloatVarPtr(AnmVm* This, float* id);
    static void WrapUVsX(RenderVertex144* vertices, int count);
    static void WrapUVsY(RenderVertex144* vertices, int count);
    static uint32_t rng(RngContext* rngContext);
    static float normalizeSigned(RngContext* rngContext);
    static float normalizeUnsigned(RngContext* rngContext);
    static float normalizeToAngle(float angle, RngContext* rngContext);
    static void loadIntoAnmVm(AnmVm* This, AnmLoaded* anmLoaded, int scriptNumber);
    static void loadAnmScript(AnmVm* This, AnmLoaded* anmLoaded, uint32_t scriptNumber);
    static void loadSingleAnmScript(AnmVm* This, AnmLoaded* anmLoaded, uint32_t scriptNumber);
    static void printD3DMatrix(const D3DMATRIX& m);
private:
    inline void loadNextInstruction()
    {
        m_currentInstruction = reinterpret_cast<AnmRawInstruction*>(
            reinterpret_cast<char*>(m_currentInstruction) + m_currentInstruction->offsetToNextInstr
        );
    }

    inline void jumpToInstruction(int address)
    {
        m_currentInstruction = reinterpret_cast<AnmRawInstruction*>(reinterpret_cast<char*>(m_beginningOfScript) + address);
    }

    inline int getIntArg(uint8_t argNumber)
    {
        int x = m_currentInstruction->args[argNumber];
        if ((m_currentInstruction->varMask & (1 << argNumber)) == 0)
            x = getIntVar(this, x);
        return x;
    }

    inline int* getIntPtrArg(uint8_t argNumber)
    {
        int* x = reinterpret_cast<int*>(&m_currentInstruction->args[argNumber]);
        if ((m_currentInstruction->varMask & (1 << argNumber)) == 0)
            x = getIntVarPtr(this, x);
        return x;
    }

    inline float getFloatArg(uint8_t argNumber)
    {
        float x = std::bit_cast<float>(m_currentInstruction->args[argNumber]);
        if ((m_currentInstruction->varMask & (1 << argNumber)) == 0)
            x = getFloatVar(this, x);
        return x;
    }

    inline float* getFloatPtrArg(uint8_t argNumber)
    {
        float* x = reinterpret_cast<float*>(&m_currentInstruction->args[argNumber]);
        if ((m_currentInstruction->varMask & (1 << argNumber)) == 0)
            x = getFloatVarPtr(this, x);
        return x;
    }
};
ASSERT_SIZE(AnmVm, 0x434);
