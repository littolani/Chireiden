#include "Supervisor.h"

int Supervisor::initializeDevices()
{
}

void Supervisor::releaseDinputIface()
{
    IDirectInput8* iDirectInput8;
    iDirectInput8 = dInputInterface;
    if (iDirectInput8)
    {
      iDirectInput8->Release();
      dInputInterface = nullptr;
    }
}

void Supervisor::enterCriticalSection(size_t i)
{
    if (i >= 12)
        return;

    if ((criticalSectionFlag & 0x8000) != 0)
    {
        EnterCriticalSection(&criticalSections[i]);
        ++criticalSectionCounters[i];
    }
}

void Supervisor::leaveCriticalSection(size_t i)
{
    if (i >= 12)
        return;

    if ((criticalSectionFlag & 0x8000) != 0)
    {
        LeaveCriticalSection(&criticalSections[i]);
        --criticalSectionCounters[i];
    }
}

