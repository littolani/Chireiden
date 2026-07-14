#include "CStreamingSound.h"

HRESULT CStreamingSound::stopSounds(CStreamingSound* This, int soundIndexMaybe)
{
    if (!This->csound.m_apDSBuffer)
        return CO_E_NOTINITIALIZED;

    This->csound.m_dsBufferDesc.lpwfxFormat = nullptr;
    This->csound.m_dsBufferDesc.guid3DAlgorithm.Data1 = 0;

    HRESULT hr = S_OK;
    for (uint32_t i = 0; i < This->csound.m_dwNumBuffers; ++i)
    {
        HRESULT hr1 = This->csound.m_apDSBuffer[i]->Stop();
        HRESULT hr2 = This->csound.m_apDSBuffer[i]->SetCurrentPosition(0);
        hr |= hr1 | hr2;
    }

    This->csound.isPlaying = FALSE;
    if (soundIndexMaybe != 0)
    {
        CWaveFile* wavFile = This->csound.m_WaveFile;
        if (wavFile && wavFile->dataSize == 1)
        {
            CloseHandle(wavFile->m_fileHandle);
            wavFile->m_fileHandle = INVALID_HANDLE_VALUE;
        }
    }
    return hr;
}

