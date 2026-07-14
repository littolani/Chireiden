#pragma once
#include "Chireiden.h"
#include "CStreamingSound.h"
#include "Macros.h"

enum SoundId
{
    NO_SOUND = -1,
    SOUND_SHOOT = 0,
    SOUND_1,
    SOUND_2,
    SOUND_3,
    SOUND_PICHUN,
    SOUND_5,
    SOUND_6,
    SOUND_7,
    SOUND_8,
    SOUND_9,
    SOUND_SELECT,
    SOUND_BACK,
    SOUND_MOVE_MENU,
    SOUND_D,
    SOUND_E,
    SOUND_F,
    SOUND_10,
    SOUND_11,
    SOUND_TOTAL_BOSS_DEATH,
    SOUND_13,
    SOUND_DAMAGE,
    SOUND_ITEM,
    SOUND_16,
    SOUND_17,
    SOUND_18,
    SOUND_19,
    SOUND_1A,
    SOUND_1B,
    SOUND_1UP,
    SOUND_TIMEOUT,
    SOUND_GRAZE,
    SOUND_POWERUP,
    SOUND_20,
    SOUND_21,
    SOUND_PAUSE,
    SOUND_SPELL_CAPTURE,
    SOUND_FAMILIAR_SPAWN,
    SOUND_DAMAGE_LOW_HEALTH,
    SOUND_TIMEOUT_2,
    SOUND_FAMILIAR_UNHIDE,
    SOUND_FAMILIAR_HIDE,
    SOUND_INVALID_ACTION,
    SOUND_42,
    SOUND_43,
    SOUND_44,
    SOUND_45,
    SOUND_46,
    SOUND_47,
    REIMU_A_GAP
};

struct SoundConfig
{
    int idk;
    short paddingMaybe;
    short bufferIndex;
};
ASSERT_SIZE(SoundConfig, 0x8);

// lots of repeated stuff going on--may be errors
class SoundManager
{
public:
    IDirectSound8* dsound;                  // <0x0>
    IDirectSoundBuffer** soundBuffer;       // <0x4>
    IDirectSoundBuffer* soundBuffersArray; // <0x8>
    CWaveFile* cwaveFile;                   // <0xc>
    int idk0[22];                           // <0x10>
    DWORD writeCursor;                      // <0x68>
    int idk3;                               // <0x6c>
    DWORD writeCursorOffset;                // <0x70>
    int idk4[101];                          // <0x74>
    IDirectSoundBuffer* dsoundBuffers[128]; // <0x208>
    uint32_t m_soundBufferIndices[128];     // <0x408>
    IDirectSoundBuffer* dSoundBuffer;       // <0x608>
    HWND hwnd;                              // <0x60c>
    IDirectSound8** dsoundIface;            // <0x610>
    DWORD threadId;                         // <0x614>
    HANDLE threadHandle;                    // <0x618>
    int idk5;                               // <0x61c>
    int m_sfxSlots[12];                     // <0x620>
    int m_sfxActiveCounts[12];              // <0x650>
    int m_sfxInstanceData[12][128];         // <0x680>
    int idk6[16];
    void* someHeapAllocatedSoundArray[16];
    int* someArray2;
    int idk8[31];
    int bgmFormatIndexMaybe;
    ThBgmFormat* bgmPreloadFmtData[16];     // <0x1f84>
    int idk9[3153];
    char bgmFilename[256];
    CStreamingSound* cStreamingSound;        // <0x5208>
    int idk11;
    int idk12;
    int someWaveFileOffset;
    HANDLE soundThread;
    HANDLE someHandle;
    DWORD soundThreadId;
    int someState;
    HWND hwnd2;
    void* sounds[47];
    int bgmVolume;
    int sfxVolume;
    int adjustedSfxVolumeMaybe;

    /**
     * 0x449660
     * Notes: Assumes 4-byte aligned or even-sized chunks
     * This code only works on LE systems.
     * RIFF/WAVE specifies that all multi-byte integer fields are LE.
     * @brief Finds a specific chunk in a RIFF/WAVE buffer.
     * @param  ECX:4        searchPtr      Pointer to the start of the chunks to search.
     * @param  Stack[0x4]:4 targetId       4-character ID to look for (e.g., "fmt " or "data").
     * @param  EAX:4        remainingSize  The total size of the sub-container to search within.
     * @param  EDI:4        outChunkSize   Receives the size of the found chunk's data.
     * @return EAX:4                       Char pointer to the chunk's data, or nullptr if not found.
     */
    static char* findRiffChunk(char* searchPtr, const char* targetId, int remainingSize, int* outChunkSize);

    static void stopBombReimuAB();

    /**
     * 0x44a1e0
     * @brief
     * @param soundIndex ESI:4
     */
    static void playSoundCentered(SoundId soundId);

    /**
     * 0x44a260
     * @brief
     * @param xOffsetFromCenter Stack[0x4]:4
     * @param soundId           EDI:4
     */
    static void playSoundWithPan(float xOffsetFromCenter, int soundId);

    // 0x449050
    static int close();

    static int releaseSounds();
    static void waitAndStopSounds(SoundManager* This);
    static int createThread(HWND window);
    static void initialize(SoundManager* This, HWND gameWindow);

    static ULONG CALLBACK loadSoundsSubroutine(LPVOID lpParameter)
    {
        SoundManager* soundManager = reinterpret_cast<SoundManager*>(lpParameter);
        soundManager->initialize(soundManager, soundManager->hwnd2);
        while (soundManager->someState == 0)
            Sleep(1);
        soundManager->sounds[0] = INVALID_HANDLE_VALUE; // VERIFY
        return 0;
    }

};
//ASSERT_SIZE(SoundManager, 0x52f4); // Verified