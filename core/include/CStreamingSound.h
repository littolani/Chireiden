#pragma once
#include "Chireiden.h"
#include "CSound.h"
#include "Macros.h"

class CStreamingSound// : public CSound
{
public:
    CSound csound; // explicit inheritance for easier size verification
    int idk0;
    IDirectSound8** dsoundIface;
    DWORD m_lastPlayPos;
    DWORD  playProgress;
    BOOL m_fillNextNotificationWithSilence;
    DWORD idk1;
    DWORD m_dwNotifySize;
    int idk2;
    int soundFlag;

    static HRESULT stopSounds(CStreamingSound* This, int soundIndexMaybe);
};
ASSERT_SIZE(CStreamingSound, 0x7c); // Size Verified