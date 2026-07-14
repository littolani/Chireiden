#include "AnmVm.h"
#include "AnmManager.h"
#include "Bomb.h"
#include "Player.h"
#include "Shottypes.h"
#include "SoundManager.h"
#include "Chain.h"

ChainCallbackResult Bomb::onTick(Bomb* This)
{
    puts("bomb on tick\n");
    if (This->isUsingBomb == FALSE)
        return ChainCallbackResult::Continue;

    int result = 0;
    switch (g_globals.character)
    {
    case Character::ReimuA:
        result = onTickReimuA(This);
        break;
    case Character::ReimuB:
        result = onTickReimuB(This);
        break;
    case Character::ReimuC:
        result = onTickReimuC(This);
        break;

    case Character::MarisaA:
        result = onTickMarisaA(This);
        break;
    case Character::MarisaB:
        result = onTickMarisaB(This);
        break;
    case Character::MarisaC:
        result = onTickMarisaC(This);
        break;
    }

    // If the bomb tick function returns non-zero, the bomb sequence has finished.
    if (result != 0)
    {
        This->isUsingBomb = FALSE;
        return ChainCallbackResult::Continue;
    }

    // Increment the bomb timer.
    // This runs if the bomb is active (isUsingBomb != 0) and did not finish this tick.
    Timer::increment(&This->timer0);
    return ChainCallbackResult::Continue;
}

int Bomb::onTickReimuA(Bomb* This)
{
    AnmVm* vm = g_anmManager->getVmById(g_anmManager, (This->vmId).id);
    if (!vm)
    {
        This->vmId.id = 0;
        g_player->vm0.loadIntoAnmVm(&g_player->vm0, g_player->playerAnm, 0);
        g_player->someFlag &= 0xfffffffd;
        g_player->attemptedVelocityInternal.x = 0;
        g_player->attemptedVelocityInternal.y = 0;
        g_player->setIframes(0x28);
        return -1;
    }

    int currentTime = This->timer0.m_current;
    if (currentTime > 179)
    {
        //if (currentTime == 180)
        //{
        //    g_soundManager.playSoundCentered(0x26);
        //    g_soundManager.stopBombReimuAB();
        //    g_player->createDamageSource(g_player, &This->playerPos, 32.0, 10.0, 0x1e, 0x32);
        //}
        //This->sub_0045df10(This);
        //(This->someVec2).x = (This->someVec2).y + (This->someVec2).x;
    }
    return 0;
}

int Bomb::onTickReimuB(Bomb* This)
{
    return 0;
}

int Bomb::onTickReimuC(Bomb* This)
{
    return 0;
}

int Bomb::onTickMarisaA(Bomb* This)
{
    return 0;
}

int Bomb::onTickMarisaB(Bomb* This)
{
    return 0;
}

int Bomb::onTickMarisaC(Bomb* This)
{
    return 0;
}

void Bomb::startReimuA(Bomb* This)
{

}

void Bomb::startReimuB(Bomb* This)
{

}

void Bomb::startReimuC(Bomb* This)
{

}

void Bomb::startMarisaA(Bomb* This)
{

}

void Bomb::startMarisaB(Bomb* This)
{

}

void Bomb::startMarisaC(Bomb* This)
{

}
