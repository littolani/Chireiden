#pragma once

struct Shooter
{
    int8_t fireRate;
    uint8_t startDelay;
    uint16_t damage;
    Float2 offset;
    Float2 hitbox;
    float angle;
    float speed;
    uint8_t option;
    char kind;
    int16_t anmScript;
    int16_t anmScriptHit;
    int16_t sfx;

    // These start as integer IDs in the file, but become function pointers?
    union { int32_t onInitId; void* onInit; }; // 0x24
    union { int32_t onTickId; void* onTick; }; // 0x28
    union { int32_t onDrawId; void* onDraw; }; // 0x2C
    union { int32_t onHitId;  void* onHit; };  // 0x30
};
ASSERT_SIZE(Shooter, 0x34);

struct ShooterList
{
    Shooter* shooters;    // File-relative offset that gets patched to an absolute pointer
    uint32_t unknown;     // 8-byte stride in the loop
};
ASSERT_SIZE(ShooterList, 0x8);

struct OptionCoordPool
{
    Float2 coords[10];
};

struct ShotFile
{
    uint16_t idk0;
    uint16_t numShooterLists;
    float hurtboxSize;
    float float_0x8;
    float itemAttractBoxSize;
    float speedSubpixel;
    float focusedSpeedSubpixel;
    float normalizedSpeedSubpixel;
    float normalizedFocusedSpeedSubpixel;
    int maxPowerMultiplier;
    int powerPerLevel;
    OptionCoordPool unfocusedPool; // 0x28
    OptionCoordPool focusedPool;   // 0x78
    OptionCoordPool unknownPool1;  // 0xC8
    OptionCoordPool unknownPool2;  // 0x118
    OptionCoordPool unknownPool3;  // 0x168
    OptionCoordPool unknownPool4;  // 0x1B8
    OptionCoordPool unknownPool5;  // 0x208
    int finalPadding[4];

    // Flexible array member starting at offset 0x268
    ShooterList shooterLists[1];
};
ASSERT_SIZE(ShotFile, 0x270);