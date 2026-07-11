#include "LaserManager.h"
#include <LaserInfinite.h>

int LaserManager::allocateNewLaser(int mode)
{
    #if 0
    LaserBase* laserBase;
    LaserBase* laserDataPtr;

    LaserManager* laserManager = g_laserManager;
    if (g_laserManager->numLasersMaybe > 256)
        return 0;

    // ??
    g_laserManager->someLimit = g_laserManager->someLimit + 1;
    if (laserManager->someLimit < 0x10000)
        laserManager->someLimit = 0x10000;

    if (mode == 0) // LaserLine
    {
        laserBase = new LaserLine;
        if (!laserBase)
            puts("Laser Allocation error");
    }
    else
    {
        if (mode != 1)
            return laserManager->someLimit;

        // LaserInfinite
        laserBase = new LaserInfinite;
        if (!laserBase)
            puts("Laser Allocation error");
    }
    laserBase->someLimit_0x6c = laserManager->someLimit;
    laserDataPtr = laserManager->laserDataPtr;
    laserBase->laserDataPtr_0x4 = (uint32_t)laserDataPtr;
    laserDataPtr->laserLineVtable_0x8 = (LaserLine_vftable *)laserLine;
    laserManager->numLasersMaybe = laserManager->numLasersMaybe + 1;
    laserManager->laserDataPtr = &laserLine->laserData;
    (*((laserLine->laserData).laserDataVtable_0x0)->vfunction2)(&laserLine->laserData);
    return laserManager->someLimit;
    #endif
    return 0;
}