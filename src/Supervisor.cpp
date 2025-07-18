#include "Supervisor.h"

Supervisor g_supervisor;

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

void Supervisor::enterCriticalSection(size_t criticalSectionNumber)
{
    if (criticalSectionNumber >= 12)
        return;

    if ((criticalSectionFlag & 0x8000) != 0)
    {
        EnterCriticalSection(&criticalSections[criticalSectionNumber]);
        ++criticalSectionCounters[criticalSectionNumber];
    }
}

void Supervisor::leaveCriticalSection(size_t criticalSectionNumber)
{
    if (criticalSectionNumber >= 12)
        return;

    if ((criticalSectionFlag & 0x8000) != 0)
    {
        LeaveCriticalSection(&criticalSections[criticalSectionNumber]);
        --criticalSectionCounters[criticalSectionNumber];
    }
}

