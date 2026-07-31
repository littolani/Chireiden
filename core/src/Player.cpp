#include "AnmManager.h"
#include "Bomb.h"
#include "BulletManager.h"
#include "Chain.h"
#include "EnemyManager.h"
#include "Globals.h"
#include "Gui.h"
#include "InputManager.h"
#include "ItemManager.h"
#include "Player.h"
#include "SoundManager.h"
#include "Spellcard.h"
#include "Shottypes.h"
#include "GeneratedSymbols.h"
#include "Vectors.h"

int Player::shootOneBullet(Player* This, Float3* position, int currentTime, Shooter* shooter)
{
    if (shooter->kind == 4 && This->shooterOptions[shooter->option] != 0)
        return 0;

    PlayerBullet* bullet = nullptr;
    int bulletIndex = 0;

    for (int i = 0; i < 256; ++i)
    {
        if (This->playerBullets[i].isActive == 0)
        {
            bullet = &This->playerBullets[i];
            bulletIndex = i + 1;
            break;
        }
    }

    if (!bullet)
        return 0; // No free bullets

    bullet->timer0.set(&bullet->timer0, 0);
    bullet->isActive = bulletIndex;
    bullet->shooter = shooter;
    bullet->damage = shooter->damage;

    if (shooter->option == 0) {
        bullet->position = *position;
    }
    else
    {
        auto& ability = This->playerAbilities[shooter->option];
        bullet->position.x = ability.someOtherFloat.x * 0.0078125f;
        bullet->position.y = ability.someOtherFloat.y * 0.0078125f;
        bullet->position.z = 0.0f;
    }

    if (shooter->kind == 4)
        This->shooterOptions[shooter->option] = bulletIndex;

    bullet->speed = shooter->speed;

    // 0x43409a: Angle Calculation
    float finalAngle;
    if (shooter->angle < 1000.0f)
    {
        if (shooter->angle < 995.0f || shooter->option == 0)
            finalAngle = shooter->angle;
        else
            finalAngle = normalizeAngle(This->playerAbilities[shooter->option].angle);
    }
    else {
        if (shooter->option == 0) {
            finalAngle = shooter->angle;
        }
        else {
            // 0x4340eb: Randomize angle
            uint32_t rngVal = AnmVm::rng(&g_replayRngContext);
            float rngFloat = (float)(int)rngVal;
            if ((int)rngVal < 0) rngFloat += 4294967296.0f;

            // GET RID OF THE COMPILER FLOAT ARTIFACTS

            float randomOffset = ((rngFloat * 4.6566129e-10f - 1.0f) * 3.1415927f) / 12.0f;
            finalAngle = randomOffset + This->playerAbilities[shooter->option].angle;

            // Normalize
            finalAngle = normalizeAngle(finalAngle);
            bullet->angle = finalAngle; // Temporary storage?

            rngVal = AnmVm::rng(&g_replayRngContext);
            rngFloat = (float)(int)rngVal;
            if ((int)rngVal < 0) rngFloat += 4294967296.0f;

            float speedMod = rngFloat * 4.6566129e-10f - 1.0f;
            bullet->speed = speedMod + speedMod + shooter->speed; // speed + 2 * random?
        }
    }

    // Normalize and store final angle
    bullet->angle = normalizeAngle(finalAngle);

    // 0x43415a: Velocity Calculation
    if ((bullet->flags & 1) == 0)
    {
        decomposeAngle(&bullet->velocity, bullet->angle, bullet->speed);
        bullet->velocity.z = 0.0f;
    }
    else {
        // Custom movement logic (smoothing)
        bullet->smth += bullet->d_smth_dt;
        float combined = bullet->speed + bullet->angle;
        bullet->angle = normalizeAngle(combined);
        // Note: Assembly path for flags & 1 seems to skip standard velocity decomposition
    }

    // 0x4341da: Apply Offset
    // Position = Position + Offset - Velocity
    bullet->position.x += shooter->offset.x - bullet->velocity.x;
    bullet->position.y += shooter->offset.y - bullet->velocity.y;

    // 0x4341fb: Create ANM Script
    // Script index is shooter->anmScript + 5
    int scriptIndex = shooter->anmScript + 5;
    AnmId vmId;

    AnmManager::makeVmWithAnmLoaded(This->playerAnm, scriptIndex, 0x16, &vmId);

    bullet->anmId4c = vmId;

    AnmVm* vm = nullptr;
    if (vmId.id != 0)
    {
        for (AnmVmListNode* node = g_anmManager->m_primaryGlobalHead; node; node = node->next)
        {
            if (node->entry->m_id.id == vmId.id) {
                vm = node->entry;
                goto VmFound;
            }
        }

        for (AnmVmListNode* node = g_anmManager->m_secondaryGlobalHead; node; node = node->next)
        {
            if (node->entry->m_id.id == vmId.id) {
                vm = node->entry;
                goto VmFound;
            }
        }
    }

    // If not found, clear IDs
    bullet->anmId4c.id = 0;
    goto PostVmLogic;

VmFound:
    if (vm->m_flagsLow & 0x8000000)
    {
        vm->m_rotation.z = shooter->angle;
        vm->m_flagsLow |= 4;
    }
    bullet->anmId50.id = 0;

    vm->m_entityPos.x = bullet->position.x + 224.0f;
    vm->m_entityPos.y = bullet->position.y + 16.0f;
    vm->m_entityPos.z = bullet->position.z;

PostVmLogic:
    // 0x434280: Run OnInit callback
    if (shooter->onInit) {
        typedef void (*ShooterCallback)(int);
        ((ShooterCallback)shooter->onInit)(currentTime);
    }

    if (shooter->sfx >= 0)
        g_soundManager.playSoundWithPan(bullet->position.x, shooter->sfx);

    return 0;
}

PlayerDamageSource* Player::createDamageSource(Player* This, Float3* v, float argF0, float argF1, int currentTime, int argi1)
{
    PlayerDamageSource* ds = This->damageSources;
    for (int i = 0; i < 32; ++i)
    {
        if ((ds->flags & 1) == 0)
        {
            memset(ds, 0, 0x74);
            ds->flags = ds->flags | 3;
            memset(&ds->centerPosition, 0, 0x34); // Okay, there's some more stuff here than just one Float3... what struct is this?
            ds->centerPosition.x = v->x;
            ds->centerPosition.y = v->y;
            ds->centerPosition.z = v->z;
            ds->argf0 = argF0;
            ds->argf1 = argF1;
            ds->timer.set(&ds->timer, currentTime);
            ds->argi1 = argi1;
            ds->someInt = 0;
            ds->someInt999999 = 999999;
            ds->someInt6c = 4;
            return ds;
        }
    }
    return ds;
}

void Player::move(Player* This)
{
    uint8_t inputFlags = static_cast<uint8_t>(g_inputManager.currentKeysDown);
    int attemptedDirection = 0;

    // Check diagonals
    if ((inputFlags & 0x50) == 0x50)
        attemptedDirection = 5;      // Up-Left
    else if ((inputFlags & 0x60) == 0x60)
        attemptedDirection = 7; // Up-Right
    else if ((inputFlags & 0x90) == 0x90)
        attemptedDirection = 6; // Down-Left
    else if ((inputFlags & 0xA0) == 0xA0)
        attemptedDirection = 8; // Down-Right
    else if ((inputFlags & 0x20) == 0)
    {
        if ((inputFlags & 0x10) == 0)
        {
            if ((inputFlags & 0x40) == 0)
            {
                if (inputFlags & 0x80)
                    attemptedDirection = 4; // Right
                else
                    attemptedDirection = 0; // Neutral
            }
            else attemptedDirection = 3; // Left
        }
        else attemptedDirection = 1; // Up
    }
    else attemptedDirection = 2; // Down

    This->attemptedDirection = attemptedDirection;

    // Focus Logic?
    // If enemy manager restricts it, or if player timer2 < 4, lock out of focus mode?
    if ((g_enemyManager == nullptr || g_enemyManager->someIndicator == 0) || (This->timer2.m_current < 4))
    {
        This->isFocused = 0;
        This->percentMovedByOptions = 0x1e;
    }
    else
    {
        This->isFocused = (g_inputManager.currentKeysDown >> 3) & 1; // Focus key (Shift)
        if (This->isFocused) puts("focused\n");
    }

    int velX = 0;
    int velY = 0;

    // Handle Reimu A's teleporting at edges (State 99 / 100)
    if (This->reimuAGappingState == 99)
        velX = -150;

    else if (This->reimuAGappingState == 100)
        velX = 150;

    else if (This->isFocused == 0) 
    {
        // Unfocused Mode
        AnmVm* vm = g_anmManager->getVmById(g_anmManager, This->anmIdFocusedHitbox.id);
        if (vm) {
            AnmManager::setInterruptById(This->anmIdFocusedHitbox.id, 1);
        }
        This->anmIdFocusedHitbox.id = 0;

        switch (attemptedDirection) {
        case 1: velY = -This->speedSubpixel; break;
        case 2: velY = This->speedSubpixel; break;
        case 3: velX = -This->speedSubpixel; break;
        case 4: velX = This->speedSubpixel; break;
        case 5: velX = -This->normalizedSpeedSubpixel; velY = -This->normalizedSpeedSubpixel; break;
        case 6: velX = This->normalizedSpeedSubpixel; velY = -This->normalizedSpeedSubpixel; break;
        case 7: velX = -This->normalizedSpeedSubpixel; velY = This->normalizedSpeedSubpixel; break;
        case 8: velX = This->normalizedSpeedSubpixel; velY = This->normalizedSpeedSubpixel; break;
        }
    }
    else 
    {
        // Focused Mode
        if (This->anmIdFocusedHitbox.id == 0)
        {
            AnmId newId;
            AnmManager::makeVmWithAnmLoaded(g_bulletManager->bulletAnm, 74, 11, &newId);
            This->anmIdFocusedHitbox.id = newId.id;
        }

        switch (attemptedDirection)
        {
        case 1:
            velY = -This->focusedSpeedSubpixel;
            break;
        case 2:
            velY = This->focusedSpeedSubpixel;
            break;
        case 3:
            velX = -This->focusedSpeedSubpixel;
            break;
        case 4:
            velX = This->focusedSpeedSubpixel;
            break;
        case 5:
            velX = -This->normalizedFocusedSpeedSubpixel;
            velY = -This->normalizedFocusedSpeedSubpixel;
            break;
        case 6:
            velX = This->normalizedFocusedSpeedSubpixel;
            velY = -This->normalizedFocusedSpeedSubpixel;
            break;
        case 7:
            velX = -This->normalizedFocusedSpeedSubpixel;
            velY = This->normalizedFocusedSpeedSubpixel;
            break;
        case 8:
            velX = This->normalizedFocusedSpeedSubpixel;
            velY = This->normalizedFocusedSpeedSubpixel;
            break;
        }
    }

    // Sprite Banking Animation
    int prevVelX = This->attemptedVelocityInternal.x;
    int scriptToLoad = -1;
    bool isUnfocusedMode = (This->someFlag & 2) == 0;

    if (isUnfocusedMode)
    {
        if (velX < 0 && prevVelX >= 0)
            scriptToLoad = 1;
        else if (velX > 0 && prevVelX <= 0)
            scriptToLoad = 3;
        else if (velX == 0)
        {
            if (prevVelX < 0)
                scriptToLoad = 2;
            else if (prevVelX > 0)
                scriptToLoad = 4;
        }
    }
    else
    {
        if (velX < 0 && prevVelX >= 0)
            scriptToLoad = 27;
        else if (velX > 0 && prevVelX <= 0)
            scriptToLoad = 29;
        else if (velX == 0)
        {
            if (prevVelX < 0)
                scriptToLoad = 28;
            else if (prevVelX > 0)
                scriptToLoad = 30;
        }
    }

    if (scriptToLoad != -1)
        AnmVm::loadIntoAnmVm(&This->vm0, This->playerAnm, scriptToLoad);

    This->attemptedVelocityInternal.x = velX;
    This->attemptedVelocityInternal.y = velY;
    int charType = g_globals.subshot + g_globals.character * 3;

    // Reimu C Speed Boost Logic
    if (charType == Character::ReimuC)
    {
        // Bit 0 (Shoot) and Bit 3 (Focus) are 0
        if ((inputFlags & 9) == 0)
        {
            This->reimuC_FramesWithout_Z_Or_Shift++;
            if (This->reimuC_FramesWithout_Z_Or_Shift > 10)
            {
                velX *= 2;
                velY *= 2;
                AnmId effectId;
                g_anmManager->makeVmWithAnmLoaded(This->playerAnm, 19, 11, &effectId);
                AnmVm* effectVm = g_anmManager->getVmById(g_anmManager, effectId.id);
                if (effectVm && effectVm->m_anmLoaded)
                    effectVm->setupTextureQuadAndMatrices(effectVm, This->vm0.m_spriteNumber, effectVm->m_anmLoaded);
                g_anmManager->setVmPosition(effectVm, &This->position);
            }
        }
        else
            This->reimuC_FramesWithout_Z_Or_Shift = 0;
    }

    // Calculate Subpixel Position Deltas
    This->attemptedDeltaPosSubpixel.x = static_cast<float>(velX) * g_gameSpeed;
    This->attemptedDeltaPosSubpixel.y = static_cast<float>(velY) * g_gameSpeed;

    if (attemptedDirection != 0)
        This->lastNonzeroAttemptedDeltaPosSubpixel = This->attemptedDeltaPosSubpixel;

    int moveX = static_cast<int>(This->attemptedDeltaPosSubpixel.x);
    int moveY = static_cast<int>(This->attemptedDeltaPosSubpixel.y);
    This->attemptedDeltaPosISubpixel.x = moveX;
    This->attemptedDeltaPosISubpixel.y = moveY;
    This->posSubpixel.x += moveX;
    This->posSubpixel.y += moveY;

    // Character Specific Ticks
    if (charType == Character::ReimuA)
        reimuATickGappingState(This); // Verified

    // Reimu B Auto-collect
    else if (charType == Character::ReimuB)
    {
        if (This->attemptedDirection == 0 && (inputFlags & 9) == 0)
        {
            This->reimuB_FramesWithoutInput++;
            if (This->reimuB_FramesWithoutInput > 10)
                g_itemManager->reimuBAutoCollect();
        }
        else
            This->reimuB_FramesWithoutInput = 0;
    }

    // Reimu A Gap Boundaries & Screen Clamp
    if (This->reimuAGappingState < 99)
    {
        if (This->posSubpixel.x < -0x5c00)
        {
            if (This->reimuAGappingState == 0 && charType == 0)
            {
                This->reimuAGappingState = 1;
                This->reimuAFramesInGappingState = 0;
            }
            This->posSubpixel.x = -0x5c00;
        }
        else if (This->posSubpixel.x > 0x5c00)
        {
            if (This->reimuAGappingState == 0 && charType == 0)
            {
                This->reimuAGappingState = 3;
                This->reimuAFramesInGappingState = 0;
            }
            This->posSubpixel.x = 0x5c00;
        }

        if (This->posSubpixel.y < 0x1000) This->posSubpixel.y = 0x1000;
        else if (This->posSubpixel.y > 0xd800) This->posSubpixel.y = 0xd800;
    }

    // Update Position Floats & Hitbox VM
    This->position.x = static_cast<float>(This->posSubpixel.x) * 0.0078125f;
    This->position.y = static_cast<float>(This->posSubpixel.y) * 0.0078125f;

    AnmVm* vm_0x756c = AnmManager::getVmById(g_anmManager, This->vm_id_0x756c.id);
    if (!vm_0x756c)
        This->vm_id_0x756c.id = 0;
    else
    {
        Float3 id_0x756c_Vec;
        id_0x756c_Vec.x = This->position.x + 224.0f; // + 32.0f + 192.0f
        id_0x756c_Vec.y = This->position.y + 16.0f;
        id_0x756c_Vec.z = This->position.z;
        g_anmManager->setVmPositionById(This->vm_id_0x756c.id, &id_0x756c_Vec);
    }

    // Marisa B Formations Update
    if (charType == 4 && !g_gui->playerRelatedValue)
    {
        if ((g_inputManager.idk3 & 0x400) != 0) { // Typical button release / transition state check
            This->marisaB_Formation = (This->marisaB_Formation + 1) % 5;
            
            if (This->reimu_c_flag_probably_0x7c90 > 0) {
                for (int i = 0; i < This->reimu_c_flag_probably_0x7c90; ++i) {
                    // setSomeVmFlagsViaAnmId(This->playerAbilities[i].anotherAnmId.id); ???
                    This->playerAbilities[i].anotherAnmId.id = 0;
                    AnmManager::makeVmWithAnmLoaded(This->playerAnm, This->marisaB_Formation + 0x22, 11, &This->playerAbilities[i].anotherAnmId);
                }
            }
        }
        
        // Assign the new formation coordinates
        for (int i = 0; i < 7; ++i)
        {
            This->playerAbilities[i].targetOffsetsUnfocused.x = This->playerAbilities[i].marisaBPreferredOffsetsByFormtion[This->marisaB_Formation].x;
            This->playerAbilities[i].targetOffsetsUnfocused.y = This->playerAbilities[i].marisaBPreferredOffsetsByFormtion[This->marisaB_Formation].y;
            This->playerAbilities[i].targetOffsetsFocused = This->playerAbilities[i].targetOffsetsUnfocused;
        }
    }

    // Player Option / Abilities Smoothing Update Loop
    if ((This->someFlag & 8) != 0)
        This->someCounter++;

    for (int i = 0; i < 8; ++i)
    {
        PlayerAbility* ability = &This->playerAbilities[i];
        if (ability->unk0[0] != 0)
        {
            if ((This->someFlag & 8) == 0)
            {
                Int2* targetOffset = This->isFocused ? &ability->targetOffsetsFocused : &ability->targetOffsetsUnfocused;
                
                ability->currentPosSubpixel.x = This->posSubpixel.x + targetOffset->x;
                ability->currentPosSubpixel.y = This->posSubpixel.y + targetOffset->y;

                if (ability->drawCallback != nullptr)
                    ability->drawCallback(ability);

                if (ability->resetFlag == 0) {
                    int percent = This->percentMovedByOptions;
                    if (percent > 0x1d) {
                        int dx = ((ability->currentPosSubpixel.x - ability->previousPosSubpixel.x) * percent) / 100;
                        int dy = ((ability->currentPosSubpixel.y - ability->previousPosSubpixel.y) * percent) / 100;
                        
                        if (dx == 0 && dy == 0) {
                            ability->previousPosSubpixel = ability->currentPosSubpixel;
                        } else {
                            ability->previousPosSubpixel.x += dx;
                            ability->previousPosSubpixel.y += dy;
                        }
                    }
                }
                else
                {
                    ability->resetFlag = 0;
                    ability->previousPosSubpixel = ability->currentPosSubpixel;
                }

                Float3 vec;
                vec.x = static_cast<float>(ability->previousPosSubpixel.x) * 0.0078125f;
                vec.y = static_cast<float>(ability->previousPosSubpixel.y) * 0.0078125f;
                vec.z = 0.0f;
                g_anmManager->setVmPositionById(ability->anotherAnmId.id, &vec);
                g_anmManager->setVmPositionById(ability->anmIds[1].id, &vec);
            }
            else {
                ability->currentPosSubpixel = This->posSubpixel;
                if (This->someCounter >= 0x1e)
                {
                    ability->unk0[0] = 0;
                    
                    if (ability->anotherAnmId.id != 0)
                    {
                        AnmVm* vm = AnmManager::getVmById(g_anmManager, ability->anotherAnmId.id);
                        if (vm)
                        {
                            vm->m_pendingInterrupt = 1;
                            if (vm->m_familyListNode.prev == nullptr)
                            {
                                for (AnmVmListNode* child = vm->m_familyListNode.next; child; child = child->next)
                                    child->entry->m_pendingInterrupt = 1;
                            }
                        }
                    }
                    if (ability->anmIds[1].id != 0)
                    {
                        AnmVm* vm = AnmManager::getVmById(g_anmManager, ability->anmIds[1].id);
                        if (vm)
                        {
                            vm->m_pendingInterrupt = 1;
                            if (vm->m_familyListNode.prev == nullptr)
                            {
                                for (AnmVmListNode* child = vm->m_familyListNode.next; child; child = child->next)
                                    child->entry->m_pendingInterrupt = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Player::setIframes(int currentTime)
{
    g_player->timerIFrames.set(&g_player->timerIFrames, currentTime);
}

int Player::useBomb()
{    
    if (g_bomb->isUsingBomb != 0)
        return -1;

    g_bomb->isUsingBomb = 1;
    g_bomb->timer0.set(&g_bomb->timer0, 0);

    if (((g_spellcard->flags & 1) == 0) || (g_spellcard->timer_0x888.m_current < 0x3c))
        g_bomb->someIndicator = 0;
    else
        g_bomb->someIndicator = 1;

    g_soundManager.playSoundWithPan(g_player->position.x, 0x26);

    //switch (g_globals.character)
    switch (g_globals.subshot + g_globals.character * 3)
    {
    case Character::ReimuA:
         g_bomb->startReimuA(g_bomb);
        break;
    case Character::ReimuB:
        g_bomb->startReimuB(g_bomb);
        break;
    case Character::ReimuC:
        g_bomb->startReimuC(g_bomb);
        break;
    case Character::MarisaA:
        g_bomb->startMarisaA(g_bomb);
        break;
    case Character::MarisaB:
        g_bomb->startMarisaB(g_bomb);
        break;
    case Character::MarisaC:
        g_bomb->startMarisaC(g_bomb);
        break;
    }
    return 0;
}

int Player::loadShotFile(Player* This, const char* filename)
{
    ShotFile* file = (ShotFile*)openFile(filename, nullptr, 0);
    This->shotFile = file;

    if (!file)
        return -1;

    for (int i = 0; i < file->numShooterLists; ++i)
    {
        // Convert file offset to absolute memory address
        uintptr_t fileBaseAddress = reinterpret_cast<uintptr_t>(file);

        file->shooterLists[i].shooters = reinterpret_cast<Shooter*>(
            reinterpret_cast<uintptr_t>(file->shooterLists[i].shooters) + fileBaseAddress
        );

        Shooter* currentShooter = file->shooterLists[i].shooters;

        // Process elements until we hit the sentinel value (fireRate < 0)
        while (currentShooter->fireRate >= 0)
        {
            // Resolve the IDs into actual function pointers from the global tables
            currentShooter->onInit = g_ShooterOnInitCallbacks[currentShooter->onInitId];
            currentShooter->onTick = g_ShooterOnTickCallbacks[currentShooter->onTickId];
            currentShooter->onDraw = g_ShooterOnDrawCallbacks[currentShooter->onDrawId];
            currentShooter->onHit = g_ShooterOnHitCallbacks[currentShooter->onHitId];
            currentShooter++;
        }
    }
    return 0;
}

int Player::initialize(Player* This)
{    
    const char* playerAnmFileName = g_globals.character == 1 ? "pl00.anm" : "pl01.anm";
    This->playerAnm = g_anmManager->preloadAnm(7, playerAnmFileName);

    if (!This->playerAnm)
    {
        puts("自機データが見つかりません。データが壊れています\n");
        return -1;
    }

    if (!g_shotFile)
    {
        int res = loadShotFile(This, g_shotFiles[static_cast<int>(g_globals.character)]);
        if (res != 0)
        {
            puts("Error loading shot file!\n");
            return -1;
        }
    }
    else
    {
        This->shotFile = g_shotFile;
        g_shotFile = nullptr;
    }

    ChainElem* chainElem = new ChainElem(onTick10Stub);
    if (!chainElem)
    {
        puts("ChainElem Allocation error!\n");
        return -1;
    }

    chainElem->args = This;
    chainElem->nextNode = (ChainElem *)((uintptr_t)chainElem->nextNode & 0xfffffffd | 1); // Indicate job node
    g_chain->registerCalcChain(chainElem, 0x10);
    This->onTick10 = chainElem;

    chainElem = new ChainElem(onDraw16Stub);
    if (!chainElem)
    {
        puts("ChainElem Allocation error!\n");
        return -1;
    }

    chainElem->args = This;
    chainElem->nextNode = (ChainElem *)((uintptr_t)chainElem->nextNode & 0xfffffffd | 1); // Indicate job node
    g_chain->registerDrawChain(chainElem, 0x16);
    This->onDraw16 = chainElem;
    
    This->vm0.initialize(&This->vm0);
    This->vm0.m_fontDimensions[1] = 16;
    This->vm0.m_fontDimensions[0] = 16;
    This->vm0.loadIntoAnmVm(&This->vm0, This->playerAnm, 0);

    ShotFile* shotFile = This->shotFile;
    This->position.x = 0.0f;
    This->position.y = 400.0f;
    This->posSubpixel.x = 0;
    This->posSubpixel.y = 51200;
    This->speedSubpixel                  = static_cast<int>(shotFile->speedSubpixel                  * 128.0);
    This->focusedSpeedSubpixel           = static_cast<int>(shotFile->focusedSpeedSubpixel           * 128.0);
    This->normalizedSpeedSubpixel        = static_cast<int>(shotFile->normalizedSpeedSubpixel        * 128.0);
    This->normalizedFocusedSpeedSubpixel = static_cast<int>(shotFile->normalizedFocusedSpeedSubpixel * 128.0);

    g_globals.maxPower = shotFile->powerPerLevel * shotFile->maxPowerMultiplier;
    g_globals.powerPerLevel = shotFile->powerPerLevel;

    This->timer0.set(&This->timer0, -1);

    This->shotFile->hurtboxSize        = g_playerHurtboxes[static_cast<int>(g_globals.character)];
    This->shotFile->itemAttractBoxSize = g_playerItemAttractBoxes[static_cast<int>(g_globals.character)];
    This->shotFile->float_0x8          = g_playerRelated_5_or_7[static_cast<int>(g_globals.character)];

    This->hurtboxHalfSize.x = This->shotFile->hurtboxSize * 0.5;
    This->hurtboxHalfSize.y = This->shotFile->hurtboxSize * 0.5;
    This->hurtboxHalfSize.z = 5.0;

    float itemAttractBox = g_playerItemAttractBoxes[static_cast<int>(g_globals.character)];
    This->itemAttractBoxUnfocusedHalfSize.y = itemAttractBox * 0.5;
    This->itemAttractBoxUnfocusedHalfSize.x = itemAttractBox * 0.5;
    This->itemAttractBoxUnfocusedHalfSize.z = 5.0;

    float itemAttractBoxFocused = g_playerItemAttractBoxesFocused[static_cast<int>(g_globals.character)];
    This->itemAttractBoxFocusedHalfSize.y = itemAttractBoxFocused * 0.5;
    This->itemAttractBoxFocusedHalfSize.x = itemAttractBoxFocused * 0.5;
    This->itemAttractBoxFocusedHalfSize.z = 5.0;

    This->hurtBox.minPos = This->position - This->hurtboxHalfSize;
    This->hurtBox.maxPos = This->hurtboxHalfSize + This->position;
    This->itemCollectBox.minPos = This->position - This->itemAttractBoxUnfocusedHalfSize;
    This->itemCollectBox.maxPos = This->itemAttractBoxUnfocusedHalfSize + This->position;
    This->itemAttractBoxFocused.maxPos = This->itemAttractBoxFocusedHalfSize + This->position;
    This->itemAttractBoxFocused.minPos = This->position - This->itemAttractBoxFocusedHalfSize;
    This->itemAttractBoxUnfocused.minPos = This->position - This->itemAttractBoxFocusedHalfSize;
    This->itemAttractBoxUnfocused.maxPos = This->itemAttractBoxFocusedHalfSize + This->position;

    This->timer1.set(&This->timer1, 0);
    This->timerIFrames.set(&This->timerIFrames, 120);

    //This->reimu_c_related_probably_0x7c90 = 0;
    This->percentMovedByOptions = 0x1e;
    return 0;
}

int Player::shootingTick(Player* This)
{
    if (This->state != 1)
    {
        This->idk7 = 0;
        This->idk8 = 0;
        return 0;
    }

    Timer* timer = &This->timer0;
    if (timer->m_current < 0) {
        if (This->reimuAGappingState > 98)
            return 0;
        //if ((g_inputManager.currentState_ & 1) == 0)
        //    return 0;
        Timer::set(&This->timer0, 0);
    }
    if (This->timer0.m_current != This->timer0.m_previous)
        shoot(This, This->timer0.m_current);

    if (0xd < This->timer0.m_current)
    {
        // Player is shooting (1 == InputBits::SHOOT)
        if ((g_inputManager.currentKeysDown & 1) != 0)
        {
            timer->addf(timer, -14.0f);
            return 0;
        }
        Timer::set(timer, -1);
        return 0;
    }
    Timer::increment(timer);
    return 0;
}

void Player::repopulateAbilities(Player* This)
{
    int characterShotType = static_cast<int>(g_globals.subshot) + static_cast<int>(g_globals.character) * 3;
    int powerLevel = g_globals.currentPower / g_globals.powerPerLevel;

    if (g_globals.currentPower >= g_globals.maxPower)
    {
        if (powerLevel > 0)
        {
            for (int i = 0; i < powerLevel; ++i)
            {
                PlayerAbility& ability = This->playerAbilities[i];

                if (ability.anmIds[1].id != 0)
                {
                    AnmVm* vm = AnmManager::getVmById(g_anmManager, ability.anmIds[1].id);
                    if (vm)
                    {
                        vm->m_flagsLow |= 0x4000000;
                        if (vm->m_familyListNode.prev == nullptr)
                        {
                            for (AnmVmListNode* child = vm->m_familyListNode.next; child; child = child->next)
                                child->entry->m_flagsLow |= 0x4000000;
                        }
                    }
                }
                ability.anmIds[1].id = 0;

                int scriptIndex = 0;
                switch (characterShotType)
                {
                case Character::ReimuA:
                    scriptIndex = 0x17;
                    ability.anmSlotIndexMaybe = This->playerAnm->m_anmSlotIndex;
                    break;
                case Character::ReimuB:
                    scriptIndex = 0x18;
                    ability.anmSlotIndexMaybe = This->playerAnm->m_anmSlotIndex;
                    break;
                case Character::ReimuC:
                    scriptIndex = 0x19;
                    // something else here??
                    // ability.anmSlotIndexMaybe = characterShotType?
                    break;
                case Character::MarisaA:
                    scriptIndex = 0x28;
                    ability.anmSlotIndexMaybe = This->playerAnm->m_anmSlotIndex;
                    break;
                case Character::MarisaB:
                    scriptIndex = 0x29;
                    ability.anmSlotIndexMaybe = This->playerAnm->m_anmSlotIndex;
                    break;
                case Character::MarisaC:
                    scriptIndex = 0x2a;
                    ability.anmSlotIndexMaybe = This->playerAnm->m_anmSlotIndex;
                    break;
                }

                if (scriptIndex != 0)
                    AnmManager::loadAnmScriptAndAddToList(This->playerAnm, scriptIndex, 11, &ability.anmIds[1]);
            }
        }
    }
    else
    {
        // Not max power
        for (int i = 0; i < 8; ++i)
        {
            PlayerAbility& ability = This->playerAbilities[i];
            if (ability.anmIds[1].id != 0)
            {
                AnmVm* vm = AnmManager::getVmById(g_anmManager, ability.anmIds[1].id);
                if (vm)
                {
                    vm->m_pendingInterrupt = 1;
                    if (vm->m_familyListNode.prev == nullptr)
                    {
                        for (AnmVmListNode* child = vm->m_familyListNode.next; child; child = child->next)
                            child->entry->m_pendingInterrupt = 1;
                    }
                }
            }
        }
    }

    if (This->reimu_c_flag_probably_0x7c90 == 0)
        This->reimuCOptionAngle = normalizeAngle(0.0f);

    if (powerLevel == This->reimu_c_flag_probably_0x7c90)
        return;

    if (powerLevel <= 0)
        goto ResetFlags;

    for (int i = 0; i < powerLevel; ++i)
    {
        PlayerAbility& ability = This->playerAbilities[i];

        ability.anmSlotIndexMaybe = 2; // Translated from playerOption_int2_64_y[-0x1a] = 2

        // NEED TO VERIFY
        int dataIndex = g_optionFormationOffsets[characterShotType][powerLevel - 1] + i;

        // Position assignments
        ability.targetAngleUnfocused = This->shotFile->unfocusedPool.coords[dataIndex].x;
        ability.targetDistanceUnfocused = This->shotFile->unfocusedPool.coords[dataIndex].y;
        ability.targetAngleFocused = This->shotFile->focusedPool.coords[dataIndex].x;
        ability.targetDistanceFocused = This->shotFile->focusedPool.coords[dataIndex].y;

        ability.targetOffsetsUnfocused.x = ability.targetAngleUnfocused * 128.0f;
        ability.targetOffsetsUnfocused.y = ability.targetDistanceUnfocused * 128.0f;

        ability.targetOffsetsFocused.x = ability.targetAngleFocused * 128.0f;
        ability.targetOffsetsFocused.y = ability.targetDistanceFocused * 128.0f;

        Int2* targetOffsets = &ability.targetOffsetsFocused;
        if (This->isFocused == 0)
            targetOffsets = &ability.targetOffsetsUnfocused;

        int targetX = This->posSubpixel.x + targetOffsets->x;
        int targetY = This->posSubpixel.y + targetOffsets->y;

        ability.currentPosSubpixel.x = targetX;
        ability.currentPosSubpixel.y = targetY;
        ability.previousPosSubpixel.x = targetX;
        ability.previousPosSubpixel.y = targetY;
        ability.optionId = i;
        
        if (ability.anmIds[0].id != 0) {
            AnmVm* vm = AnmManager::getVmById(g_anmManager, ability.anmIds[0].id);
            if (vm) {
                vm->m_flagsLow |= 0x4000000;
                if (vm->m_familyListNode.prev == nullptr)
                    for (AnmVmListNode* child = vm->m_familyListNode.next; child; child = child->next)
                        child->entry->m_flagsLow |= 0x4000000;
            }
        }
        ability.anmIds[0].id = 0;

        switch (g_globals.subshot + g_globals.character * 3)
        {
        case Character::ReimuA:
            // ability.drawCallback = reinterpret_cast<void(*)(PlayerAbility*)>(0x4336a0); // REPLACE LATER
            ability.updateCallback = reinterpret_cast<void(*)(PlayerAbility*)>(0x433690);
            ability.someAngleFloat = -D3DX_PI / 2;
            ability.angle = normalizeAngle((i * 2 * D3DX_PI) / (float)powerLevel - D3DX_PI / 2);
            AnmManager::makeVmWithAnmLoaded(This->playerAnm, 0x14, 11, &ability.anmIds[0]);
            break;

        case Character::ReimuB:
            //ability.updateCallback = abilityCallbackReimuB; // 0x4337b0
            AnmManager::makeVmWithAnmLoaded(This->playerAnm, 0x15, 11, &ability.anmIds[0]);
            break;

        case Character::ReimuC:
            //ability.updateCallback = abilityCallbackReimuC;
            AnmManager::makeVmWithAnmLoaded(This->playerAnm, 0x16, 11, &ability.anmIds[0]);
            break;

        case Character::MarisaA:
            ability.updateCallback = abilityCallbackMarisaA; // 0x433990
            {
                int scriptNumber = g_optionFormationOffsets[5][i + powerLevel * 8 + 8] + 0x15; // THIS CAN READ OUTSIDE THE BOUNDS
                AnmManager::makeVmWithAnmLoaded(This->playerAnm, scriptNumber, 11, &ability.anmIds[0]);
            }
            break;

        case Character::MarisaB:
            // Marisa B pulls from extra coordinate pools to switch between formations?
            ability.marisaBPreferredOffsetsByFormtion[0].x = This->shotFile->unfocusedPool.coords[dataIndex].x * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[0].y = This->shotFile->unfocusedPool.coords[dataIndex].y * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[1].x = This->shotFile->focusedPool.coords[dataIndex].x * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[1].y = This->shotFile->focusedPool.coords[dataIndex].y * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[2].x = This->shotFile->unknownPool1.coords[dataIndex].x * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[2].y = This->shotFile->unknownPool1.coords[dataIndex].y * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[3].x = This->shotFile->unknownPool2.coords[dataIndex].x * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[3].y = This->shotFile->unknownPool2.coords[dataIndex].y * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[4].x = This->shotFile->unknownPool3.coords[dataIndex].x * 128.0f;
            ability.marisaBPreferredOffsetsByFormtion[4].y = This->shotFile->unknownPool3.coords[dataIndex].y * 128.0f;

            //ability.updateCallback = abilityCallbackMarisaB; // 0x433a50
            AnmManager::makeVmWithAnmLoaded(This->playerAnm, This->marisaB_Formation + 0x22, 11, &ability.anmIds[0]);
            break;

        case Character::MarisaC:
            //ability.updateCallback = abilityCallbackMarisaC; // 0x433b20
            AnmManager::makeVmWithAnmLoaded(This->playerAnm, 0x27, 11, &ability.anmIds[0]);
            break;
        }
    }

ResetFlags:
    This->reimu_c_flag_probably_0x7c90 = powerLevel;
    for (int i = powerLevel; i < 8; ++i)
    {
        PlayerAbility& ability = This->playerAbilities[i];
        ability.anmSlotIndexMaybe = 0;
        if (ability.anmIds[0].id != 0)
        {
            AnmVm* vm = AnmManager::getVmById(g_anmManager, ability.anmIds[0].id);
            if (vm)
            {
                vm->m_pendingInterrupt = 1;
                if (vm->m_familyListNode.prev == nullptr)
                    for (AnmVmListNode* child = vm->m_familyListNode.next; child; child = child->next)
                        child->entry->m_pendingInterrupt = 1;
            }
        }
    }
    
    for (int i = 0; i < 8; ++i)
        This->playerAbilities[i].resetFlag = 1;
}

void Player::shoot(Player* This, int currentTime)
{
}

void Player::abilityDrawCallbackReimuA(PlayerAbility* ability)
{
    Float2 outVec;
    float orbitRadius = g_player->isFocused ? 24.0f : 64.0f;
    decomposeAngle(&outVec, ability->angle,orbitRadius);

    ability->currentPosSubpixel.y = g_player->posSubpixel.x - static_cast<int>(outVec.x * -128.0);
    ability->previousPosSubpixel.x = g_player->posSubpixel.y - static_cast<int>(outVec.y * -128.0);

    // Rotate the orbs
    ability->angle = normalizeAngle(ability->angle + 0.10471976);

    int i = ability->currentPosSubpixel.y;
    if (i < -0x6600)
    {
        ability->idk8 |= 1;
        ability->resetFlag = 1;
        ability->currentPosSubpixel.y = i + 0xcc00;
        return;
    }
    if (i > 0x6600)
    {
        ability->idk8 |= 1;
        ability->resetFlag = 1;
        ability->currentPosSubpixel.y = i + -0xcc00;
        return;
    }
    if (ability->idk8 & 1U)
        ability->resetFlag = 1;

    ability->idk8 = ability->idk8 & 0xfffffffe;
}

void Player::abilityCallbackReimuB(PlayerAbility* ability)
{
    
}

void Player::abilityCallbackReimuC(PlayerAbility* ability)
{
    
}

void Player::abilityCallbackMarisaA(PlayerAbility* ability)
{
    
}

void Player::abilityCallbackMarisaB(PlayerAbility* ability)
{
    
}

void Player::abilityCallbackMarisaC(PlayerAbility* ability)
{
    
}

void Player::reimuATickGappingState(Player* This)
{
    if (This->reimuAGappingState == 0)
        return;

    // Guard Condition: Determine if the gap state should be canceled
    // The state shouldn't be canceled if Reimu is actively teleporting (state 99 or 100)
    bool shouldCancel = false;
    if (This->reimuAGappingState <= 98)
    {
        bool holdingShootOrFocus = (g_inputManager.currentKeysDown & (InputBits::SHOOT | InputBits::FOCUS)) != 0;
        bool isDialogActive = g_gui->playerRelatedValue != 0;
        bool bossIndicatorMissing = (g_enemyManager != nullptr && g_enemyManager->someIndicator == 0);

        if (holdingShootOrFocus || isDialogActive || bossIndicatorMissing)
            shouldCancel = true;
    }

    // Cancel the gap wind-up and reset
    if (shouldCancel)
    {
        This->reimuAGappingState = 0;
        This->reimuAFramesInGappingState = 0;
        ++This->reimuAFramesInGappingState;
        return;
    }

    // State Machine
    switch (This->reimuAGappingState)
    {
        // Left Gap Windup
        case 1:
            if (This->attemptedDirection == 0) // Neutral input
            {
                This->reimuAFramesInGappingState = 0;
                This->reimuAGappingState = 2;
            }
            else if (This->attemptedDirection != 3) // 3 = Left
                This->reimuAGappingState = 0;
            break;

        case 2:
            if (This->reimuAFramesInGappingState < 9)
            {
                // Must double-tap left (3) within 9 frames
                if (This->attemptedDirection == 3 && g_gui->playerRelatedValue == 0 &&
                    g_enemyManager != nullptr && g_enemyManager->someIndicator != 0) {
                    
                    This->reimuAGappingState = 99; // Transition to active Left Gap
                    This->reimuAFramesInGappingState = 0;
                    SoundManager::playSoundCentered(SoundId::SOUND_47);
                    repopulateAbilities(This);
                }
                break;
            }
            // Timeout: Fallthrough to reset if > 9 frames
            This->reimuAGappingState = 0;
            This->reimuAFramesInGappingState = 0;
            break;

        // Right Gap Windup
        case 3:
            if (This->attemptedDirection == 0) { // Neutral input
                This->reimuAFramesInGappingState = 0;
                This->reimuAGappingState = 4;
            } else if (This->attemptedDirection != 4) { // 4 = Right
                This->reimuAGappingState = 0;
            }
            break;

        case 4:
            if (This->reimuAFramesInGappingState < 9)
            {
                // Must double-tap right (4) within 9 frames
                if (This->attemptedDirection == 4 && !g_gui->playerRelatedValue && 
                    g_enemyManager && g_enemyManager->someIndicator)
                {
                    
                    This->reimuAGappingState = 100; // Transition to active Right Gap
                    This->reimuAFramesInGappingState = 0;
                    SoundManager::playSoundCentered(SoundId::REIMU_A_GAP);
                    repopulateAbilities(This);
                }
                break;
            }
            // Timeout: Fallthrough to reset if > 9 frames
            This->reimuAGappingState = 0;
            This->reimuAFramesInGappingState = 0;
            break;

        // Active gap state (Left)
        case 99:

            if (This->posSubpixel.x < -0x67FF)
            {
                This->posSubpixel.x += 0xD000; 
                for (int i = 0; i < 8; ++i)
                    This->playerAbilities[i].resetFlag = 1;
            }
            
            // State lasts for 45 frames
            if (This->reimuAFramesInGappingState >= 45)
            {
                This->reimuAGappingState = 0;
                This->reimuAFramesInGappingState = 0;
            }
            break;

        // Active gap state (Right)
        case 100:
            if (This->posSubpixel.x > 0x67FF)
            {
                This->posSubpixel.x -= 0xD000;
                for (int i = 0; i < 8; ++i)
                    This->playerAbilities[i].resetFlag = 1;
            }
            
            if (This->reimuAFramesInGappingState >= 45)
            {
                This->reimuAGappingState = 0;
                This->reimuAFramesInGappingState = 0;
            }
            break;

        default:
            break;
    }
    ++This->reimuAFramesInGappingState;
}