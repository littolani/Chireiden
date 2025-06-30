#pragma once

#include "AnmVm.h"
#include "AnmLoaded.h"
#include "Chireiden.h"
#include "Chain.h"
#include "FileAbstrction.h"
#include "Macros.h"

struct RenderVertex144
{
    D3DXVECTOR4 transformedPosition;
    D3DCOLOR color;
    D3DXVECTOR2 textureUv;
};
ASSERT_SIZE(RenderVertex144, 0x1c);

struct AnmVertexBuffers
{
    int leftoverSpriteCount;
    RenderVertex144 spriteVertexData[0x20000];
    RenderVertex144* spriteWriteCursor;
    RenderVertex144* spriteRenderCursor;
};
ASSERT_SIZE(AnmVertexBuffers, 0x38000c);

struct BlitParams
{
    int anmLoadedIndex;
    int anmLoadedD3dIndex;
    RECT srcRect;
    RECT destRect;
};
ASSERT_SIZE(BlitParams, 0x28);

class AnmManager
{
public:
    int unk0;
    uint32_t unk1[42];
    int refreshCounter;
    int vectorOffset0;
    int vectorOffset1;
    int unk3;
    AnmVm bulkVms[4096]; // <0xbc>
    uint8_t bulkVmsIsAlive[4096];
    int nextBulkVmIndex;
    AnmLoaded* loadedAnms[32]; // <0x4350c0>
    D3DXMATRIX matrix0;
    AnmVm primaryVm;
    uint32_t u0;
    uint32_t u1;
    IDirect3DBaseTexture9** lplpBaseTexture;
    uint16_t p;
    uint8_t haveFlushedSprites;
    uint8_t stuff[9];
    IDirect3DVertexBuffer9* vertexBuffer;
    D3DXVECTOR3 primitive0Position;
    D3DXVECTOR2 primitive0Uv;
    D3DXVECTOR3 primitive1Position;
    D3DXVECTOR2 primitive1Uv;
    D3DXVECTOR3 primitive2Position;
    D3DXVECTOR2 primitive2Uv;
    D3DXVECTOR3 primitive3Position;
    D3DXVECTOR2 primitive3Uv;
    AnmVertexBuffers vertexBuffers;
    AnmVmList* primaryGlobalNext;
    AnmVmList* primaryGlobalPrev;
    AnmVmList* secondaryGlobalNext;
    AnmVmList* secondaryGlobalPrev;
    AnmVm vmLayers[31];
    uint32_t q[3];

    void preloadAnm(int anmIdx,char *anmFileName);
    void preloadAnmFromMemory(char* anmFilePath, int anmSlotIndex);
    void markAnmLoadedAsReleasedInVmList(AnmLoaded* anmLoaded);
    void releaseTextures();
    void flushSprites();
    void blitTextureToSurface(BlitParams* blitParams);
    AnmVm* allocateVm();
private:
    static constexpr int NUM_BULK_VMS = 4096;
    static constexpr int NUM_ANM_LOADEDS = 32;

    inline int getNextBulkVmIndex(int current) { return (current + 1) & (NUM_BULK_VMS - 1); }
};
ASSERT_SIZE(AnmManager, 0x7bd894);


