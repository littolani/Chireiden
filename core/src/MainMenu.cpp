#include "AnmManager.h"
#include "InputManager.h"
#include "MainMenu.h"
#include "SoundManager.h"

void MainMenu::sub_00459130(MenuSelection* menuSelect)
{
    int* stackDepth;

    stackDepth = &menuSelect->stackDepth;
    *stackDepth = *stackDepth + -1;
    if (*stackDepth < 0)
        menuSelect->stackDepth = 0;

    menuSelect->nextSelection = menuSelect->stackSelection[menuSelect->stackDepth];
    menuSelect->numChoices = menuSelect->stackNumChoices[menuSelect->stackDepth];
    menuSelect->disabledIndicesCount = 0;
    return;
}

int MenuSelection::seek(MenuSelection* This, int offset)
{
    int numChoices = This->numChoices;

    if (numChoices > 0)
    {
        bool isSelectionDisabled;
        do
        {
            This->nextSelection += offset;

            // Handle Overflow
            while (numChoices <= This->nextSelection)
            {
                if (This->isWrapEnabled == 0)
                    This->nextSelection = numChoices - 1; // Clamp to bottom
                else
                    This->nextSelection -= numChoices; // Wrap to top
            }

            // Handle Underflow
            while (This->nextSelection < 0)
            {
                if (This->isWrapEnabled == 0)
                    This->nextSelection = 0; // Clamp to top
                else
                    This->nextSelection += numChoices; // Wrap to bottom
            }

            isSelectionDisabled = false;
            for (int skipIndex = 0; skipIndex < This->disabledIndicesCount; ++skipIndex)
            {
                if (This->disabledIndices[skipIndex] == This->nextSelection)
                {
                    isSelectionDisabled = true;
                    break;
                }
            }
        } while (isSelectionDisabled);
    }
    return This->nextSelection;
}

void MainMenu::setMode(MainMenu* This, uint32_t value)
{
    This->currentMenuMode = value;
    This->timer2.set(&This->timer2, 0);
}

int MainMenu::func3(MainMenu* This)
{
#if 0
    AnmLoaded* anmLoaded;
    AnmVm* vm;
    AnmId local_4;
    MenuSelection* menuSelection;
    AnmVmListNode* node;

    switch (This->currentMenuMode)
    {
    case 0:
        This->menuSelect.numChoices = 5;
        int numChoices = This->menuSelect.numChoices;
        if (numChoices == 0)
            This->menuSelect.nextSelection = 0;
        else if (numChoices < 1)
            This->menuSelect.nextSelection = numChoices + -1;
        else
            This->menuSelect.nextSelection = 0;

        anmLoaded = This->titleAnm;
        g_anmManager->makeVmWithAnmLoaded(anmLoaded, 1, 0x16, &local_4);
        This->anmId2.id = anmLoaded->m_anmSlotIndex;
        volumeRelated(This);
        setMode(This, 1);
    case 1:
        if (6 < This->timer2.m_current)
        {
            setMode(This, 2);
            vmInterruptThing((This->anmId2).id, 3);
            AnmManager::setInterruptById((This->anmId2).id, (short)(This->menuSelect).nextSelection + 0x11);
            return 1;
        }
        break;
    case 2:
        menuSelection = &This->menuSelect;
        (This->menuSelect).currentSelection = (This->menuSelect).nextSelection;
        if (((g_inputManager.triggerState & 0x10U) != 0) ||
            (((byte)g_inputManager.autoRepeatState & 0x10) != 0)) {
            MenuSelection::seek(menuSelection, -1);
        }
        if (((g_inputManager.triggerState & 0x20U) != 0) ||
            (((byte)g_inputManager.autoRepeatState & 0x20) != 0)) {
            MenuSelection::seek(menuSelection, 1);
        }
        if ((This->menuSelect).currentSelection != menuSelection->nextSelection) {
            g_soundManager.playSoundCentered(0xc);
            vmInterruptThing((This->anmId2).id, 3);
            AnmManager::setInterruptById((This->anmId2).id, (short)menuSelection->nextSelection + 7);
        }
        if ((g_inputManager.triggerState & 0x102U) != 0) {
            if (menuSelection->nextSelection != 4) {
                SoundManager::playSoundCentered(0xb);
                nextSelection = (This->menuSelect).numChoices;
                if (nextSelection == 0) {
                    menuSelection->nextSelection = 4;
                }
                else if (nextSelection < 5) {
                    menuSelection->nextSelection = nextSelection + -1;
                }
                else {
                    menuSelection->nextSelection = 4;
                }
                vmInterruptThing((This->anmId2).id, 3);
                AnmManager::setInterruptById((This->anmId2).id, (short)menuSelection->nextSelection + 7);
                return 1;
            }
        LAB_0043a97d:
            vm = AnmManager::getVmWithId(g_anmManager, (This->anmId2).id);
            if ((vm != (AnmVm*)0x0) &&
                (node = (vm->m_familyListNode).prev, vm->m_pendingInterrupt = 6,
                    node == (AnmVmListNode*)0x0)) {
                for (node = (vm->m_familyListNode).next; node != (AnmVmListNode*)0x0; node = node->next) {
                    node->entry->m_pendingInterrupt = 6;
                }
            }
            nextSelection = 0xb;
        LAB_0043aa4a:
            SoundManager::playSoundCentered(nextSelection);
            setMode(This, 4);
            return 1;
        }
        if ((menuSelection->nextSelection == 1) &&
            (nextSelection = timer_isMultipleOf(&This->timer2, 0x3c), nextSelection != 0)) {
            /* the volume preview thingy */
            SoundManager::playSoundCentered(SOUND_PICHUN);
        }
        if ((g_inputManager.triggerState & 0x40U) || (g_inputManager.autoRepeatState & 0x40))
        {
            if (menuSelection->nextSelection == 0)
            {
                if (g_supervisor.m_gameConfig.bgmVolume < 5)
                    g_supervisor.m_gameConfig.bgmVolume = 0;
                else
                    g_supervisor.m_gameConfig.bgmVolume -= 5;
            }
            else
            {
                if (menuSelection->nextSelection != 1)
                    goto LAB_0043a8ff;
                if (g_supervisor.m_gameConfig.sfxVolume < 5)
                    g_supervisor.m_gameConfig.sfxVolume = 0;
                else
                    g_supervisor.m_gameConfig.sfxVolume += 251;
            }
            volumeRelated(This);
        }
    LAB_0043a8ff:
        if (((g_inputManager.triggerState & 0x80U) != 0) ||
            (((byte)g_inputManager.autoRepeatState & 0x80) != 0)) {
            if (menuSelection->nextSelection == 0)
            {
                g_supervisor.m_gameConfig.bgmVolume += 5;
                if (g_supervisor.m_gameConfig.bgmVolume > 100)
                    g_supervisor.m_gameConfig.bgmVolume = 100;
            }
            else
            {
                if (menuSelection->nextSelection != 1)
                    goto LAB_0043a953;
                g_supervisor.m_gameConfig.sfxVolume += 5;
                if (g_supervisor.m_gameConfig.sfxVolume > 100)
                    g_supervisor.m_gameConfig.sfxVolume = 100;
            }
            volumeRelated(This);
        }
    LAB_0043a953:
        if ((g_inputManager.triggerState & 0x80001U) != 0) {
            nextSelection = menuSelection->nextSelection;
            if (nextSelection == 2) {
                vm = AnmManager::getVmWithId(g_anmManager, (This->anmId2).id);
                if ((vm != (AnmVm*)0x0) &&
                    (node = (vm->m_familyListNode).prev, vm->m_pendingInterrupt = 6,
                        node == (AnmVmListNode*)0x0)) {
                    for (node = (vm->m_familyListNode).next; node != (AnmVmListNode*)0x0; node = node->next)
                    {
                        node->entry->m_pendingInterrupt = 6;
                    }
                }
                nextSelection = 10;
                goto LAB_0043aa4a;
            }
            /* reset */
            if (nextSelection == 3)
            {
                g_supervisor.m_gameConfig.bgmVolume = 100;
                g_supervisor.m_gameConfig.sfxVolume = 80;
                g_supervisor.m_gameConfig.isWindowed = 0;
                volumeRelated(This);
                SoundManager::playSoundCentered(SOUND_SELECT);
                return 1;
            }
            if (nextSelection == 4)
                goto LAB_0043a97d;
        }
        break;
    case 3:
        break;
    case 4:
        if (This->timer2.m_current > 9)
        {
            nextSelection = (This->menuSelect).nextSelection;
            if (nextSelection == 2) {
                resetTimer(4, This);
                someSubshotThing((Subshot*)&This->menuSelect);
            }
            else if (nextSelection == 4) {
                resetTimer(1, This);
                sub_00459130(&This->menuSelect);
                return 1;
            }
        }
        break;
    default:
        goto switchD_0043a703_caseD_5;
    }
switchD_0043a703_caseD_5:
#endif
    return 1;

}

int MainMenu::customizeOptionsMenu(MainMenu* This, uint8_t di)
{
#if 0
    AnmLoaded* titleAnm;
    AnmVm* vm;
    int key;
    AnmId local_4;
    MenuSelection* menuSelection;
    int numChoices;
    AnmVmListNode* vmPrev;

    switch (This->currentMenuMode)
    {
    case 0:
        (This->menuSelect).numChoices = 7;
        numChoices = (This->menuSelect).numChoices;
        if (numChoices == 0) {
            (This->menuSelect).nextSelection = 0;
        }
        else if (numChoices < 1) {
            (This->menuSelect).nextSelection = numChoices + -1;
        }
        else {
            (This->menuSelect).nextSelection = 0;
        }
        titleAnm = This->titleAnm;
        AnmManager::makeVmWithAnmLoaded(titleAnm, 2, 0x16, &local_4);
        This->anmId3.id = titleAnm->m_anmSlotIndex;
        setMode(1, This);
        This->shootKey = g_defaultGameConfig.shootKey;
        This->bombKey = g_defaultGameConfig.bombKey;
        This->focusKey = g_defaultGameConfig.focusKey;
        This->pauseKey = g_defaultGameConfig.pauseKey;
        This->skipKey = g_defaultGameConfig.skipKey;
        matchVmWithScriptNumber(This);
    case 1:
        if (6 < (This->timer2).current) {
            setMode(2, This);
            vmInterruptThing((This->anmId3).id, 3);
            AnmManager::setInterruptById((This->anmId3).id, (short)(This->menuSelect).nextSelection + 0x11);
            return 1;
        }
        break;
    case 2:
        menuSelection = &This->menuSelect;
        (This->menuSelect).currentSelection = (This->menuSelect).nextSelection;
        if (((g_inputManager.triggerState & 0x10U) != 0) ||
            ((g_inputManager.autoRepeatState & 0x10) != 0)) {
            menuSelection->seek(menuSelection, -1);
        }
        if (((g_inputManager.triggerState & 0x20U) != 0) ||
            ((g_inputManager.autoRepeatState & 0x20) != 0)) {
            menuSelection->seek(menuSelection, 1);
        }
        if ((This->menuSelect).currentSelection != menuSelection->nextSelection) {
            SoundManager::playSoundCentered(0xc);
            vmInterruptThing((This->anmId3).id, 3);
            AnmManager::setInterruptById((This->anmId3).id, (short)menuSelection->nextSelection + 7);
        }
        numChoices = jooystickStuff_004575a0();
        key = 0;
        do {
            if ((*(byte*)(key + numChoices) & 0x80) != 0) {
                if (menuSelection->nextSelection < 5) {
                    sub_0043c6f0(This, key, menuSelection->nextSelection);
                }
                break;
            }
            key = key + 1;
        } while (key < 31);
        if (((g_inputManager.triggerState & 0x102U) != 0) && (menuSelection->nextSelection == 6))
        {
            This->shootKey = g_defaultGameConfig.shootKey;
            This->bombKey = g_defaultGameConfig.bombKey;
            This->focusKey = g_defaultGameConfig.focusKey;
            This->pauseKey = g_defaultGameConfig.pauseKey;
            This->skipKey = g_defaultGameConfig.skipKey;
            matchVmWithScriptNumber(This);
        LAB_0043bc59:
            SoundManager::playSoundCentered(0xb);
            vm = AnmManager::getVmWithId(g_anmManager, (This->anmId3).id);
            if ((vm != (AnmVm*)0x0) &&
                (vmPrev = (vm->m_familyListNode).prev, vm->m_pendingInterrupt = 6,
                    vmPrev == (AnmVmListNode*)0x0)) {
                for (vmPrev = (vm->m_familyListNode).next; vmPrev != (AnmVmListNode*)0x0;
                    vmPrev = vmPrev->next) {
                    vmPrev->entry->m_pendingInterrupt = 6;
                }
            }
            setMode(This, 4);
            return 1;
        }
        if ((g_inputManager.triggerState & 0x80001U) != 0) {
            if (menuSelection->nextSelection == 5)
            {
                This->shootKey = g_defaultGameConfig.shootKey;
                This->bombKey = g_defaultGameConfig.bombKey;
                This->focusKey = g_defaultGameConfig.focusKey;
                This->pauseKey = g_defaultGameConfig.pauseKey;
                This->skipKey = g_defaultGameConfig.skipKey;
                matchVmWithScriptNumber(This);
                SoundManager::playSoundCentered(10);
                return 1;
            }
            if (menuSelection->nextSelection == 6)
            {
                /* default config pressed? */
                g_supervisor.m_gameConfig.shootKey = This->shootKey;
                g_supervisor.m_gameConfig.bombKey = This->bombKey;
                g_supervisor.m_gameConfig.focusKey = This->focusKey;
                g_supervisor.m_gameConfig.pauseKey = This->pauseKey;
                g_supervisor.m_gameConfig.skipKey = This->skipKey;
                g_supervisor.m_gameConfig.upKey = g_defaultGameConfig.upKey;
                g_supervisor.m_gameConfig.downKey = g_defaultGameConfig.downKey;
                g_supervisor.m_gameConfig.leftKey = g_defaultGameConfig.leftKey;
                g_supervisor.m_gameConfig.rightKey = g_defaultGameConfig.rightKey;
                g_defaultGameConfig.bombKey = g_supervisor.m_gameConfig.bombKey;
                g_defaultGameConfig.focusKey = g_supervisor.m_gameConfig.focusKey;
                g_defaultGameConfig.pauseKey = g_supervisor.m_gameConfig.pauseKey;
                g_defaultGameConfig.upKey = g_supervisor.m_gameConfig.upKey;
                g_defaultGameConfig.downKey = g_supervisor.m_gameConfig.downKey;
                g_defaultGameConfig.leftKey = g_supervisor.m_gameConfig.leftKey;
                g_defaultGameConfig.rightKey = g_supervisor.m_gameConfig.rightKey;
                g_defaultGameConfig.skipKey = g_supervisor.m_gameConfig.skipKey;
                goto LAB_0043bc59;
            }
        }
        break;
    case 3:
        break;
    case 4:
        if (9 < This->timer2.m_current) {
            resetTimer(3, This);
            sub_00459130(&This->menuSelect);
        }
        break;
    default:
        goto switchD_0043b9e0_caseD_5;
    }
switchD_0043b9e0_caseD_5:
#endif
    return 1;
}