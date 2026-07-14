#pragma once
#include "Chireiden.h"
#include "GeneratedSymbols.h"
#include "Shottypes.h"
#include "Timer.h"
#include "ThunkGenerator.h"
#include "Vectors.h"

struct Globals
{
    int scoreLimit;
    int currentScore;
    int currentPower;
    int u1;
    int currentPiv;
    int u2;
    int u3;
    Timer timer;
    int character;
    int subshot;
    int currentLives;
    int currentLifeFragments;
    int difficulty;
    int u4;
    int currentStage;
    int currentStageCopy;
    int u5;
    int timeInStage;
    int timeInChapter;
    int continuesUsed;
    int alsoContinuesUsed;
    int rank;
    int maxPower;
    int powerPerLevel;
    int livesMaybe;
    int graze;
};
ASSERT_SIZE(Globals, 0x78);

//X_FUNC(void, game_free, 0x45fd49, (void* memory))
//X_FUNC(void*, game_malloc, 0x460192, (size_t size))
//X_FUNC(void*, game_new, 0x45FCE4, (size_t size))

inline auto game_free = reinterpret_cast<void(*)(void*)>(0x45FD49);
inline auto game_malloc = reinterpret_cast<void* (*)(size_t)>(0x460192);
inline auto game_new = reinterpret_cast<void* (*)(size_t)>(0x45FCE4);

extern D3DFORMAT g_d3dFormats[];
extern uint32_t g_bytesPerPixelLookupTable[];

int fileExists(LPCSTR filePath);
int createDirectory(LPCSTR pathName);
int writeToFile(LPCSTR fileName, DWORD numBytes, LPVOID bytes);

// 0x40b9d0
void projectMagnitudeToVectorComponents(D3DXVECTOR3* vec, float theta, float scale);

/**
 * 0x458400
 * @brief  Returns a memory-mapped file
 * @param  filename           EAX:4
 * @param  outSize            Stack[0x4]:4
 * @param  isExternalResource Stack[0x8]:4
 * @return byte*              EAX:4
 */
byte* openFile(const char* filename, size_t* outSize, BOOL isExternalResource);

/**
 * 0x458400
 * @brief  Returns an angle in the range [-PI, PI]
 * @param  inputAngleRadians
 * @param  isExternalResource Stack[0x8]:4
 * @return float
 */
float normalizeAngle(float inputAngleRadians);

// 0x459aa0
void decomposeAngle(Float2* outVec, float angle, float scale);

void decomposeAngle(Float3* outVec, float angle, float scale);

extern const char* g_shotFiles[];
extern float g_playerHurtboxes[2];
extern float g_playerItemAttractBoxes[2];
extern float g_playerItemAttractBoxesFocused[2];
extern float g_playerRelated_5_or_7[2];