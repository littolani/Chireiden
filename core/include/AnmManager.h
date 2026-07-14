#pragma once
#include "Chireiden.h"
#include "AnmVm.h"
#include "FileAbstraction.h"

struct AnmLoaded;
struct AnmLoadedD3D;
struct AnmHeader;

struct BlitParams
{
    int anmLoadedIndex;
    int anmLoadedD3dIndex;
    RECT srcRect;
    RECT destRect;
};
ASSERT_SIZE(BlitParams, 0x28);

struct AnmVertexBuffers
{
    int leftoverSpriteCount;
    RenderVertex144 spriteVertexData[0x20000];
    RenderVertex144* spriteWriteCursor;
    RenderVertex144* spriteRenderCursor;
};
ASSERT_SIZE(AnmVertexBuffers, 0x38000c);

struct Rect3D
{
    Float3 bottomLeft;
    Float3 bottomRight;
    Float3 topRight;
    Float3 topLeft;
};

class AnmManager
{
public:
    BlitParams m_blitParamsArray[4];               // <0x0>
    int m_allocatedVmCountMaybe;                   // <0xa0>
    int m_idk;                                     // <0xa4>
    int m_someCounter;                             // <0xa8>
    int m_refreshCounter;                          // <0xac>
    float m_globalRenderQuadOffsetX;               // <0xb0>
    float m_globalRenderQuadOffsetY;               // <0xb4>
    int m_someTickCounter;                         // <0xb8>
    AnmVm m_fastVms[4096];                         // <0xbc>
    uint8_t m_fastVmsIsAlive[4096];                // <0x4340bc>
    int m_nextFastVmIndex;                         // <0x4350bc>
    AnmLoaded* m_loadedAnms[32];                   // <0x4350c0>
    D3DXMATRIX m_currentWorldMatrix;               // <0x435140>
    AnmVm m_primaryVm;                             // <0x435180>
    uint32_t m_u0;                                 // <0x4355bc>
    D3DCOLOR m_color;                              // <0x4355b8>
    AnmLoadedD3D* m_anmLoadedD3D;                  // <0x4355bc>
    uint8_t m_renderStateMode;                     // <0x4355c0>
    int8_t m_unk_4355c1;                           // <0x4355c1>
    int8_t m_haveFlushedSprites;                   // <0x4355c2>
    uint8_t m_unk_4355c3;                          // <0x4355c3>
    uint8_t m_unk_4355c4;                          // <0x4355c4>
    uint8_t m_unk_4355c5;                          // <0x4355c5>
    bool m_usePointFilter;                         // <0x4355c6>
    uint8_t m_idk2;                                // <0x4355c7>
    AnmLoadedSprite* m_cachedSprite;               // <0x4355c8>
    IDirect3DVertexBuffer9* m_squareVertexBuffer;  // <0x4355cc>
    RenderVertexSq m_squareVertices[4];            // <0x4355d0>
    AnmVertexBuffers m_anmVertexBuffers;           // <0x435620>
    AnmVmListNode* m_primaryGlobalHead;            // <0x7b562c>
    AnmVmListNode* m_primaryGlobalTail;            // <0x7b5630>
    AnmVmListNode* m_secondaryGlobalHead;          // <0x7b5634>
    AnmVmListNode* m_secondaryGlobalTail;          // <0x7b5638>
    AnmVm m_vmLayers[31];                          // <0x7b563c>
    int m_id;                                      // <0x7bd888>
    uint8_t m_scaleB;                              // <0x7bd88c>
    uint8_t m_scaleG;                              // <0x7bd88d>
    uint8_t m_scaleR;                              // <0x7bd88e>
    uint8_t m_scaleA;                              // <0x7bd88f>
    uint32_t m_rebuildColorFlag;                   // <0x7bd890>

    AnmManager();
    ~AnmManager();

    /**
     * 0x44fd10
     * @brief Renders any batched sprites that have been queued up but not yet drawn, then resets the batch state.
     * @param This ESI:4
     */
    static void flushSprites(AnmManager* This);

    /**
     * 0x44f710
     * @brief Extracts render state parameters from vm flags and synchronizes them with D3D (Identical to applyRenderStateForVm)
     * @param This EAX:4
     * @param vm   EDI:4
     */
    static void setupRenderStateForVm(AnmManager* This, AnmVm* vm);

    /**
     * 0x44f4b0
     * @brief Extracts render state parameters from VM flags and synchronizes them with D3D (Identical to setupRenderStateForVm)
     * @param This EAX:4
     * @param vm   Stack[0x4]:4
     */
    static void applyRenderStateForVm(AnmManager* This, AnmVm* vm);

    /**
     * 0x44f880
     * @brief Draws VM sprite on a layer
     * @param This  ECX:4
     * @param layer Stack[0x4]:4
     * @param vm    EAX:4
     */
    static void drawVmSprite2D(AnmManager* This, uint32_t layer, AnmVm* anmVm);

    /**
     * 0x44fda0
     * @brief Writes sprite vertex buffers
     * @param This         EAX:4
     * @param vertexBuffer EDX:4
     */
    static void writeSprite(AnmManager* This, RenderVertex144* vertexBuffer);

    /**
     * 0x4543e0
     * @brief Function to process a single ANM chunk
     * @param  anmLoaded    ECX:4
     * @param  anmHeader    Stack[0x4]:4
     * @param  chunkIndex   EBX:4
     * @return int          EAX:4
     */
    static int openAnmLoaded(AnmLoaded* anmLoaded, AnmHeader* anmHeader, int chunkIndex);

    /**
     * 0x454190
     * @brief
     * @param  This         Stack[0x4]:4
     * @param  anmSlotIndex Stack[0x8]:4
     * @param  anmFilePath  ECX:4
     * @return AnmLoaded*   EAX:4
     */
    static AnmLoaded* preloadAnmFromMemory(AnmManager* This, int anmSlotIndex, const char* anmFilePath);

    /**
     * 0x454360
     * @brief
     * @param  anmSlotIndex ECX:4
     * @param  anmFilePath  EBX:4
     * @return AnmLoaded*   EAX:4
     */
    static AnmLoaded* preloadAnm(int anmSlotIndex, const char* anmFileName);

    /**
     * 0x450e20
     * @brief
     * @param  This  Stack[0x4]:4
     * @param  vm    EAX:4
     * @return int   EAX:4
     */
    static int updateWorldMatrixAndProjectQuadCorners(AnmManager* This, AnmVm* vm);

    /**
     * 0x450b00
     * @brief
     * @param  This  EDI:4
     * @param  vm    ESI:4
     * @return int   EAX:4
     */
    static int drawVmWithFog(AnmManager* This, AnmVm* vm);

    /**
     * 0x451ef0
     * @brief
     * @param  This  ECX:4
     * @param  vm    EAX:4
     */
    static void drawVm(AnmManager* This, AnmVm* vm);

    /**
     * 0x4513a0
     * @brief
     * @param  This  Stack[0x4]:4
     * @param  vm    EBX:4
     * @return int   EAX:4
     */
    static int drawVmWithTextureTransform(AnmManager* This, AnmVm* vm);

    /**
     * 0x453220
     * @brief s
     */
    static void setupRenderSquare();

    // 0x455a00
    static void makeVmWithAnmLoaded(AnmLoaded* anmLoaded, int scriptNumber, int anmVmLayer, AnmId* anmId);
    static void removeVm(AnmManager* This, AnmVm* vm);
    static void addVm(AnmVm* vm, AnmId* anmId);
    static void createD3DTextures(AnmManager* This);
    static void markAnmLoadedAsReleasedInVmList(AnmManager* This, AnmLoaded* anmLoaded);
    static void releaseTextures();
    static void blitTextureToSurface(AnmManager* This, BlitParams* blitParams);
    
    static void transformAndDraw(AnmManager* This, AnmVm* vm);
    static void loadIntoAnmVm(AnmVm* vm, AnmLoaded* anmLoaded, int scriptNumber);
    static void putInVmList(AnmVm* vm, AnmId* anmId);
    static void releaseAnmLoaded(AnmManager* This, AnmLoaded* anmLoaded);
    
    static void drawRect(AnmVm* vm, Rect3D* rect3d);
    static int drawVmTriangleStrip(AnmManager* This, AnmVm* vm, SpecialRenderData* specialRenderData, uint32_t vertexCount);
    static int drawVmTriangleFan(AnmManager* This, AnmVm* vm, SpecialRenderData* specialRenderData, uint32_t vertexCount);

    static void setInterruptById(int vmId, uint16_t pendingInterruptFlag);

    /**
     * 0x4569c0
     * @brief Finds and returns a fast vm (preallocated) or allocates a new one if no space is available
     */
    static AnmVm* allocateVm();
    static AnmManager* initialize(AnmManager* This);
    static AnmVm* getVmById(AnmManager* This, int anmId);
    static void blitTextures(AnmManager* This);
    static void spawnVmAtPosition(AnmLoaded* anmLoaded, AnmId* anmId, int scriptNumber, int layer, Float3* spawnLocation);
    static void setVmPosition(AnmVm* vm, Float3* position);
    static void setVmPositionById(int id, Float3* position);
    static void loadAnmScriptAndAddToList(AnmLoaded* anmLoaded, uint32_t scriptNumber, int layer, AnmId* outAnmId);

    static ChainCallbackResult renderLayer(AnmManager* This, int layer);

    static ChainCallbackResult __fastcall onTick1a(void* args);
    static ChainCallbackResult __fastcall onTick08(void* args);
    static ChainCallbackResult __fastcall on_draw_04_just_renders_layer_00(void* args);
    static ChainCallbackResult __fastcall on_draw_06_just_renders_layer_01(void* args);
    static ChainCallbackResult __fastcall on_draw_08_just_renders_layer_02(void* args);
    static ChainCallbackResult __fastcall on_draw_0a_also_renders_layer_03(void* args);
    static ChainCallbackResult __fastcall on_draw_0c_just_renders_layer_04(void* args);
    static ChainCallbackResult __fastcall on_draw_0f_just_renders_layer_05(void* args);
    static ChainCallbackResult __fastcall on_draw_10_just_renders_layer_06(void* args);
    static ChainCallbackResult __fastcall on_draw_11_just_renders_layer_07(void* args);
    static ChainCallbackResult __fastcall on_draw_12_just_renders_layer_08(void* args);
    static ChainCallbackResult __fastcall on_draw_13_just_renders_layer_09(void* args);
    static ChainCallbackResult __fastcall on_draw_15_just_renders_layer_10(void* args);
    static ChainCallbackResult __fastcall on_draw_17_just_renders_layer_11(void* args);
    static ChainCallbackResult __fastcall on_draw_18_just_renders_layer_12(void* args);
    static ChainCallbackResult __fastcall on_draw_1a_just_renders_layer_13(void* args);
    static ChainCallbackResult __fastcall on_draw_1c_just_renders_layer_14(void* args);
    static ChainCallbackResult __fastcall on_draw_20_just_renders_layer_15(void* args);
    static ChainCallbackResult __fastcall on_draw_22_just_renders_layer_16(void* args);
    static ChainCallbackResult __fastcall on_draw_24_just_renders_layer_17(void* args);
    static ChainCallbackResult __fastcall on_draw_27_just_renders_layer_18(void* args);
    static ChainCallbackResult __fastcall on_draw_28_also_renders_layer_19(void* args);
    static ChainCallbackResult __fastcall on_draw_32_just_renders_layer_22(void* args);
    static ChainCallbackResult __fastcall on_draw_3c_just_renders_layer_23(void* args);
    static ChainCallbackResult __fastcall on_draw_3d_just_renders_layer_24(void* args);
    static ChainCallbackResult __fastcall on_draw_31_just_renders_layer_21(void* args);
    static ChainCallbackResult __fastcall on_draw_30_also_renders_layer_20(void* args);
    static ChainCallbackResult __fastcall on_draw_3f_also_renders_layer_29(void* args);
    static ChainCallbackResult __fastcall on_draw_3e_also_renders_layer_30(void* args);

private:
    static constexpr int NUM_FAST_VMS = 4096;
    static constexpr int NUM_ANM_LOADEDS = 32;
    static uint32_t modulateColorComponent(uint16_t base, uint16_t factor);

    static inline int getNextFastVmIndex(int current)
    {
        return (current + 1) & (NUM_FAST_VMS - 1);
    }

    static inline uint8_t scaleChannel(uint32_t scale, uint8_t v)
    {
        uint32_t x = (scale * v) >> 7;
        x = std::min(x, uint32_t{ 255 });
        return static_cast<uint8_t>(x);
    }
};
ASSERT_SIZE(AnmManager, 0x7bd894);
