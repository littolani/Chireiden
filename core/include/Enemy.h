#pragma once
#include "Chireiden.h"
#include "EclVm.h"
#include "EnemyBase.h"
#include "Macros.h"

class Enemy : public EclVm
{
public:
    EclVm eclVm;
    EnemyBase enemyBase;

    //virtual void __thiscall vfunction1() override;
    //virtual void __thiscall vfunction2() override;
    //virtual void __thiscall vfunction3() override;
    //virtual void __thiscall vfunction4() override;
    //virtual void __thiscall vfunction5() override;

    static EclRunContext* getEclRunContextViaAsyncId(Enemy* This, int asyncId);

};