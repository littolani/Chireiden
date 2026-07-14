#pragma once
#include "AnmVm.h"
#include "Bullet.h"
#include "Chain.h"
#include "Chireiden.h"
#include "Macros.h"

class BulletManager
{
public:
    int idk0;
    int idk1;
    ChainElem* onCalcChain;
    ChainElem* onDrawChain;
    Bullet* nextFreeBullet;
    Bullet* bulletDrawList[6];
    int idk[14];
    Bullet bullets[2000];
    Bullet dummyBullet;
    AnmLoaded* bulletAnm;
};
ASSERT_SIZE(BulletManager, 0x46d678);