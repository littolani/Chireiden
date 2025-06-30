#include "Chireiden.h"
#include "Globals.h"

// Structure for an ANM file chunk (based on Touhou Toolkit's anm_header11_t)
struct AnmHeader
{
    uint32_t version;        // Offset 0x00
    uint16_t numSprites;     // Offset 0x04
    uint16_t numScripts;     // Offset 0x06
    uint16_t zero1;          // Offset 0x08
    uint16_t w;              // Offset 0x0A
    uint16_t h;              // Offset 0x0C
    uint16_t format;         // Offset 0x0E
    uint32_t nameOffset;     // Offset 0x10
    uint16_t x;              // Offset 0x14
    uint16_t y;              // Offset 0x16
    uint32_t memoryPriority; // Offset 0x18
    uint32_t thtxOffset;     // Offset 0x1C
    uint16_t hasData;        // Offset 0x20
    uint16_t lowresScale;    // Offset 0x22
    // uint16_t jpeg_quality;   // Offset 0x24 (Note: nextoffset is at 0x24 in assembly, adjusting here)
    uint32_t nextOffset;     // Offset 0x24
    uint16_t wMax;          // Offset 0x28
    uint16_t hMax;          // Offset 0x2A
};
ASSERT_SIZE(AnmHeader, 0x2c);

// Structure for chunk data stored in AnmLoaded offset 0x120 (20 bytes per entry)
struct AnmLoadedD3D
{
    IDirect3DTexture9* m_texture; // 0x0
    void* m_srcData; // 0x4
    uint32_t m_srcDataSize; // 0x8
    int m_bytesPerPixel; // 0xc
    int m_flags;

    int createTextureFromAtR();
    int createTextureFromAt(uint32_t width, uint32_t height, int formatIndex);
    int createTextureFromThtx(uint32_t texWidth, uint32_t texHeight, int formatIndex, void* thtxData);
    int createTextureFromImage(int textureWidthOffset, uint32_t width, uint32_t height, int formatIndex);
};
ASSERT_SIZE(AnmLoadedD3D, 0x14);

struct AnmLoaded
{
    int anmSlotIndex;           // Offset 0x00
    char filePath[260];         // Offset 0x04  (assuming typical MAX_PATH size)
    AnmHeader* header;          // Offset 0x108
    int numAnmLoadedD3Ds;       // Offset 0x10c (numHeaders)
    int numScripts;             // Offset 0x110 (numScripts)
    int numSprites;             // Offset 0x114 (numSprites)
    void* keyframeData;         // Offset 0x118 (keyframeData)
    void* spriteData;           // Offset 0x11c (spriteData)
    AnmLoadedD3D* anmLoadedD3D; // Offset 0x120 (AnmLoadedD3DBuffer)
    int anmsLoaded;             // Offset 0x124
    int unknown;
    int texturesCreated;
    void* unknownHeapAllocated;

    void release();
    void setupTextures();
    int createTextureForEntry(int i, int numSprites, int numScripts, AnmHeader* anmHeader);
};
ASSERT_SIZE(AnmLoaded, 0x134);

struct AnmLoadedSprite
{
    int sourceFileIndex;
    D3DXVECTOR2 startPixel;
    D3DXVECTOR2 endPixel;
    float textureHeight;
    float textureWidth;
    D3DXVECTOR2 uvStart;
    D3DXVECTOR2 uvEnd;
    float height;
    float width;
    int spriteId;
};

