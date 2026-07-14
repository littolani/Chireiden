#include "SoundManager.h"
#include "Globals.h"

char* SoundManager::findRiffChunk(char* searchPtr, const char* targetId, int remainingSize, int* outChunkSize)
{
    if (remainingSize != 0)
    {
        do {
            // The size of the current chunk is at offset 4
            int currentChunkSize = *(int*)(searchPtr + 4);
            *outChunkSize = currentChunkSize;

            // Compare the 4-character ID at offset 0
            if (strncmp(searchPtr, targetId, 4) == 0)           
                return searchPtr + 8; // Return pointer to the data (skipping ID and Size fields)

            // Move to the next chunk: current pointer + 8-byte header + data size
            searchPtr += (currentChunkSize + 8);
            remainingSize -= (currentChunkSize + 8);

        } while (remainingSize != 0);
    }

    return nullptr;
}

// 0x44a300
void SoundManager::stopBombReimuAB()
{
    SoundManager* This = &g_soundManager;
    const int BOMBLOOP_LOGICAL_ID = 0x31; // 0x31 is the Logical ID for the Reimu AB Bomb Loop.

    int* pSlotId = This->m_sfxSlots;
    int slotIndex = 0;

    // Try to find the running loop instance
    while (*pSlotId != -1)
    {
        if (*pSlotId == BOMBLOOP_LOGICAL_ID)
            This->m_sfxActiveCounts[slotIndex] = -1; // Setting active count to -1 signals the mixer to release the buffer immediately.

        pSlotId++;
        slotIndex++;

        if (slotIndex >= 12)
            return; // Not found, nothing to kill
    }

    // If we didn't find it playing, but we have a free slot,
    // claim the slot for ID 0x31 anyway.
    if (slotIndex < 12)
        This->m_sfxSlots[slotIndex] = BOMBLOOP_LOGICAL_ID;
}

void SoundManager::playSoundCentered(SoundId soundId)
{
    printf("Playing sound %d centered\n", soundId);
    SoundManager* This = &g_soundManager;
    int16_t bufferConfig = g_soundConfigTable[soundId].bufferIndex;

    int slotIndex = 0;
    int* pSlotId = This->m_sfxSlots;

    while (true)
    {
        int currentSlotId = *pSlotId;

        // Found empty slot
        if (currentSlotId < 0)
        {
            if (slotIndex < 12)
            {
                This->m_sfxSlots[slotIndex] = soundId;
                This->m_soundBufferIndices[soundId] = bufferConfig;
                This->m_sfxInstanceData[slotIndex][0] = 0;
                This->m_sfxActiveCounts[slotIndex]++;
            }
            return;
        }

        // This sound is already active in This slot. We just add another instance (polyphony).
        if (currentSlotId == soundId)
            break;

        pSlotId++;
        slotIndex++;

        // End of array check (12 slots max)
        if (slotIndex >= 12)
            return; // No room left in the queue, sound is dropped.
    }

    // Check polyphony limit (Max 128 instances per sound)
    int currentCount = This->m_sfxActiveCounts[slotIndex];
    if (currentCount >= 128)
        return; // Too many copies of This sound playing. Drop it.

    This->m_sfxInstanceData[slotIndex][currentCount] = 0;
    This->m_sfxActiveCounts[slotIndex]++;
}

void SoundManager::playSoundWithPan(float xOffsetFromCenter, int soundId)
{
    //printf("Playing sound %d at offset %f\n", soundId, xOffsetFromCenter);

    SoundManager* This = &g_soundManager;

    // Get the buffer index (WAV file ID) from the config table
    short bufferIndex = g_soundConfigTable[soundId].bufferIndex;

    // Calculate Stereo Panning
    // Map the Game World X (-192 to +192) to DirectSound Pan (-1000 to +1000)
    // 192.0f is the half-width of the gameplay field.
    int panValue = static_cast<int>((xOffsetFromCenter * 1000.0f) / 192.0f);
    int slotIndex = 0;
    int* pSlotId = This->m_sfxSlots;

    while (true)
    {
        // Found an empty slot (-1)
        if (*pSlotId < 0)
        {
            if (slotIndex < 12)
            {
                // Claim slot
                This->m_sfxSlots[slotIndex] = soundId;

                // Store Physical Buffer ID
                This->m_soundBufferIndices[soundId] = (int)bufferIndex;

                // Store the Calculated Pan Value
                // This array stores the "instance data" (Pan) for the active sound
                This->m_sfxInstanceData[slotIndex][0] = panValue;
                This->m_sfxActiveCounts[slotIndex]++;
            }
            return;
        }

        // Sound is already playing in this slot, add polyphonic instance
        if (*pSlotId == soundId)
            break;

        pSlotId++;
        slotIndex++;
        if (slotIndex >= 12)
            return;
    }

    // Check Polyphony limit (128 instances)
    if (This->m_sfxActiveCounts[slotIndex] >= 128)
        return;

    // Add new instance with the calculated Pan
    This->m_sfxInstanceData[slotIndex][This->m_sfxActiveCounts[slotIndex]] = panValue;
    This->m_sfxActiveCounts[slotIndex]++;
}

#if 0
HRESULT initCStreamingSound(CStreamingSound** destPtr, IDirectSound8** lplpdsound8, GUID guid3D, int dwDsBufferSize, HANDLE bgmUpdateCallback, ThBgmFormat* thBgmFormat)
{
    if (*lplpdsound8 == nullptr)
        return DSERR_UNINITIALIZED;

    CWaveFile* pWaveFile = (CWaveFile*)game_new(sizeof(CWaveFile));
    if (!pWaveFile)
        return E_OUTOFMEMORY;

    memset(pWaveFile, 0, sizeof(CWaveFile));
    pWaveFile->dataSize = 1;

    HRESULT hr = pWaveFile->open(pWaveFile, thBgmFormat, "thbgm.dat");
    if (FAILED(hr))
    {
        if (pWaveFile->dataSize == 1 && pWaveFile->fileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(pWaveFile->fileHandle);
            pWaveFile->fileHandle = INVALID_HANDLE_VALUE;
        }
        game_free(pWaveFile);
        return E_FAIL;
    }

    DSBUFFERDESC dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize = sizeof(DSBUFFERDESC);

    // Flags 0x18188:
    dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE;

    dsbd.dwBufferBytes = dwDsBufferSize * 16;
    dsbd.lpwfxFormat = (WAVEFORMATEX*)&pWaveFile->thBgmFormat->format;
    dsbd.guid3DAlgorithm = guid3D;

    IDirectSoundBuffer* pDSBuffer = nullptr;
    IDirectSoundNotify* pDSNotify = nullptr;

    // 0x459f7c: CreateSoundBuffer
    hr = (*lplpdsound8)->CreateSoundBuffer(&dsbd, &pDSBuffer, nullptr);
    if (FAILED(hr)) {
        game_free(pWaveFile);
        return E_FAIL;
    }

    // 0x459f8d: QueryInterface for IDirectSoundNotify
    // Assembly queries pDSBuffer for IID_IDirectSoundNotify (g_dsoundGuid)
    hr = pDSBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&pDSNotify);
    if (FAILED(hr)) {
        pDSBuffer->Release();
        game_free(pWaveFile);
        return E_FAIL;
    }

    // 0x459fb0: Allocate notification positions (16 entries)
    // 16 * 8 bytes = 128 bytes (0x80)
    DSBPOSITIONNOTIFY* pNotifies = (DSBPOSITIONNOTIFY*)game_new(16 * sizeof(DSBPOSITIONNOTIFY));
    if (pNotifies == nullptr) {
        pDSNotify->Release();
        pDSBuffer->Release();
        game_free(pWaveFile);
        return E_FAIL;
    }

    // 0x45a090: Fill notification positions
    // Triggers at the end of every `dwDsBufferSize` chunk
    for (int i = 0; i < 16; ++i) {
        pNotifies[i].dwOffset = (dwDsBufferSize - 1) + (i * dwDsBufferSize);
        pNotifies[i].hEventNotify = bgmUpdateCallback;
    }

    // 0x45a0ac: SetNotificationPositions
    hr = pDSNotify->SetNotificationPositions(16, pNotifies);

    // 0x45a0d1: Cleanup notifications and interface
    game_free(pNotifies);
    pDSNotify->Release(); // Interface released after setting positions

    if (FAILED(hr)) {
        pDSBuffer->Release();
        game_free(pWaveFile);
        return E_OUTOFMEMORY;
    }

    // 0x45a0fc: Create CStreamingSound
    CStreamingSound* pStreamSound = (CStreamingSound*)game_new(sizeof(CStreamingSound));
    if (pStreamSound == nullptr) {
        pDSBuffer->Release();
        game_free(pWaveFile);
        return E_FAIL;
    }

    // 0x45a11f: Call Constructor
    // Using placement new assuming the user has the constructor available/linked
    new (pStreamSound) CStreamingSound(pDSBuffer, dwDsBufferSize * 16, pWaveFile, dwDsBufferSize);

    // 0x45a12d: Copy DSBUFFERDESC into the new object
    pStreamSound->csound.m_dsBufferDesc = dsbd;

    // 0x45a13e: Set remaining members
    pStreamSound->dsoundIface = lplpdsound8;
    pStreamSound->bgmUpdateCallback = bgmUpdateCallback;
    pStreamSound->soundFlag = 0;

    // 0x45a128: Assign to output pointer
    *destPtr = pStreamSound;

    return S_OK;
}
#endif

int SoundManager::close()
{
    if (g_soundManager.soundThread)
    {
        if (g_soundManager.someState == 0)
            g_soundManager.someState = 1;

        DWORD waitResult = WaitForSingleObject(g_soundManager.soundThread, 100);
        while (waitResult == WAIT_TIMEOUT)
        {
            Sleep(1);
            waitResult = WaitForSingleObject(g_soundManager.soundThread, 100);
        }

        waitResult = WaitForSingleObject(g_soundManager.someHandle, 100);
        while (waitResult == WAIT_TIMEOUT)
        {
            Sleep(1);
            waitResult = WaitForSingleObject(g_soundManager.someHandle, 100);
        }
        CloseHandle(g_soundManager.soundThread);
        CloseHandle(g_soundManager.someHandle);
        g_soundManager.soundThread = NULL;
        g_soundManager.someHandle = NULL;
    }
    return 0;
}

int SoundManager::releaseSounds()
{
    SoundManager* This = &g_soundManager;
    if (This->bgmPreloadFmtData[0] != nullptr)
    {
        free(This->bgmPreloadFmtData[0]);
        This->bgmPreloadFmtData[0] = nullptr;
    }

    for (int i = 0; i < 128; ++i)
    {
        if (This->dsoundBuffers[i] != nullptr)
        {
            This->dsoundBuffers[i]->Release();
            This->dsoundBuffers[i] = nullptr;
        }

        if (This->soundBuffersArray != nullptr)
        {
            This->soundBuffersArray[i].Release();
            IDirectSoundBuffer** soundBuffersArray = &soundBuffersArray[i];
            soundBuffersArray = nullptr;
        }
    }

    for (int i = 0; i < 46; ++i)
    {
        if (This->sounds[i] != nullptr)
        {
            free(This->sounds[i]);
            This->sounds[i] = nullptr;
        }
    }

    if (This->dsoundIface != nullptr)
    {
        KillTimer(This->hwnd, 1);
        waitAndStopSounds(This);

        This->dsound = nullptr;

        if (This->dSoundBuffer != nullptr)
        {
            This->dSoundBuffer->Stop();
            This->dSoundBuffer->Release();
        }

        if (This->cStreamingSound != nullptr)
        {
            game_free(&This->cStreamingSound); //TODO: VERIFY CORRECT FREE FUNCTION
            This->cStreamingSound = nullptr;
        }

        if (This->dsoundIface != nullptr)
        {
            if (*This->dsoundIface != nullptr)
            {
                (*This->dsoundIface)->Release();
                *This->dsoundIface = nullptr;
            }
            game_free(This->dsoundIface);
            This->dsoundIface = nullptr;
        }
        for (int i = 0; i < 16; ++i)
        {
            if (This->someHeapAllocatedSoundArray[i] != nullptr)
            {
                free(This->someHeapAllocatedSoundArray[i]);
                This->someHeapAllocatedSoundArray[i] = nullptr;
            }
        }
    }
    return 0;
}

void SoundManager::waitAndStopSounds(SoundManager* This)
{
    DWORD waitStatus;
    if (This->cStreamingSound)
    {
        This->cStreamingSound->stopSounds(This->cStreamingSound, 1);
        if (This->threadHandle)
        {
            PostThreadMessageA(This->threadId, 0x12, 0, 0);
            waitStatus = WaitForSingleObject(This->threadHandle, 0x100);
            while (waitStatus)
            {
                PostThreadMessageA(This->threadId, 0x12, 0, 0);
                waitStatus = WaitForSingleObject(This->threadHandle, 0x100);
            }
            CloseHandle(This->threadHandle);
            CloseHandle((HANDLE)This->idk11);
            This->threadHandle = (HANDLE)0x0;
        }
        CStreamingSound* cStreamingSnd = This->cStreamingSound;
        delete This->cStreamingSound; //TODO: NEED TO FIX CALL
        This->cStreamingSound = nullptr;
    }
}

int SoundManager::createThread(HWND window)
{
    memset(&g_soundManager, 0, sizeof(SoundManager)); // 0x52f4
    g_soundManager.hwnd2 = window;
    g_soundManager.soundThread = CreateThread(
        NULL,
        0,
        loadSoundsSubroutine,
        &g_soundManager,
        0,
        &g_soundManager.soundThreadId);
    return 0;
}

void SoundManager::initialize(SoundManager* This, HWND gameWindow)
{
#if 0
    for (int i = 0; i < 0x80; i++)
        This->m_soundBufferIndices[i] = -1;

    for (int i = 0; i < 12; i++)
        This->m_sfxSlots[i] = -1;

    IDirectSound8** pDSound = (IDirectSound8**)game_new(sizeof(IDirectSound8*));
    if (pDSound != nullptr)
        *pDSound = nullptr;

    This->dsoundIface = pDSound;

    if (*pDSound != nullptr)
    {
        (*pDSound)->Release();
        *pDSound = nullptr;
    }

    HRESULT hr = DirectSoundCreate8(nullptr, pDSound, nullptr);
    if (FAILED(hr) || FAILED((*pDSound)->SetCooperativeLevel(gameWindow, DSSCL_PRIORITY)))
    {

        puts("DirectSound オブジェクトの初期化が失敗したよ\n");

        if (This->dsoundIface != nullptr)
        {
            if (*This->dsoundIface != nullptr)
            {
                (*This->dsoundIface)->Release();
                *This->dsoundIface = nullptr;
            }
            free(This->dsoundIface);
            This->dsoundIface = nullptr;
        }
        return;
    }

    directSoundBufferDescStuff(pDSound);
    This->dsound = *This->dsoundIface;

    // Set up standard 44.1kHz, 16-bit, Stereo PCM Wave Format
    WAVEFORMATEX waveFormat = { 0 };
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.nAvgBytesPerSec = 176400;
    waveFormat.nBlockAlign = 4;
    waveFormat.wBitsPerSample = 16;
    waveFormat.cbSize = 0;

    DSBUFFERDESC dSoundBufferDesc = { 0 };
    dSoundBufferDesc.dwSize = sizeof(DSBUFFERDESC); // 0x24
    dSoundBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS; // 0x8008
    dSoundBufferDesc.dwBufferBytes = 0x8000;
    dSoundBufferDesc.lpwfxFormat = &waveFormat;

    This->threadHandle = nullptr;

    hr = This->dsound->CreateSoundBuffer(&dSoundBufferDesc, &This->dSoundBuffer, nullptr);
    if (SUCCEEDED(hr))
    {
        LPVOID audioPtr1 = nullptr;
        DWORD audioBytes1 = 0;
        LPVOID audioPtr2 = nullptr;
        DWORD audioBytes2 = 0;

        hr = This->dSoundBuffer->Lock(0, 0x8000, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0);
        if (SUCCEEDED(hr))
        {
            // Zero out the buffer to prevent garbage noise, then unlock
            memset(audioPtr1, 0, 0x8000);
            This->dSoundBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2);

            // Play looping (1 = DSBPLAY_LOOPING)
            This->dSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

            This->bgmVolume = 100;
            This->sfxVolume = 100;
            This->hwnd = gameWindow;

            // Set up a 250ms window timer
            SetTimer(gameWindow, 0, 250, nullptr);

            // Load individual sound files
            int soundIndex = 0;
            while (g_soundManager.someState != 2)
            {
                int loadResult = loadSoundByFilename(&g_soundManager, This->dSoundBuffer, soundIndex, g_soundFiles[soundIndex]);

                if (loadResult != 0)
                {
                    printf("Sound ファイルが読み込めない データを確認 %s\n", g_soundFiles[soundIndex]);
                    return;
                }

                soundIndex++;

                // Break once all 46 files are loaded
                if (soundIndex > 46)
                {
                    puts("DirectSound は正常に初期化されました\n");
                    break;
                }
            }
        }
    }
#endif
}