#pragma once
#include "AnmLoaded.h"
#include "Chain.h"
#include "Chireiden.h"
#include "Macros.h"
#include "Timer.h"

struct MenuSelection
{
    int nextSelection;
    int currentSelection;
    int numChoices;
    int stackSelection[16];
    int stackNumChoices[16];
    int stackDepth;
    int disabledIndices[16];
    int isWrapEnabled;
    int disabledIndicesCount;

    static int seek(MenuSelection* This, int offset);
};
ASSERT_SIZE(MenuSelection, 0xd8);

class ReplayManager;
class MainMenu
{
public:
    void* vtable;
    uint32_t currentMenu;
    uint32_t previousMenu;
    ChainElem* calcChain;
    ChainElem* drawChain;
    AnmLoaded* titleAnm;
    AnmLoaded* titlevAnm;
    uint32_t drawFlag;
    uint32_t currentMenuMode;
    MenuSelection menuSelect;
    MenuSelection axis2Control;
    MenuSelection axis3Control;
    int k;
    Timer timer2;
    AnmId anmId0;
    AnmId anmId2;
    AnmId anmId3;
    int idk12[81];
    int asciiAnmSlotIndex;
    int idk13[7];
    AnmId anmId4;
    AnmId anmId5;
    int idk14[6];
    int titleAnmSlotIndex2;
    int idk15[49];
    int arr_0x51c;
    int idk16;
    int arr_0x524;
    int idk17[5];
    int anmSlotArr_0x53c;
    int idk18[7];
    int titleAnmSlotIndex3;
    int idk19[26];
    int asciiAnmSlotIndex2;
    int int_0x5cc;
    AnmId anmId;
    int idk20[14];
    AnmId anmIds_;
    int idk21[28];
    int numCursorPositions;
    int numMenuOptions;
    int idk22;
    BOOL returnToTop;
    int massive_arr[1040];
    int i;
    int massive_arr2[1281];
    int a[500];
    int b[500];
    int c[500];
    int d[500];
    int e[500];
    int s[100];
    int x[100];
    int w[134];
    char q[92];
    int f[85];
    int someInterpYScale;
    int df[6];
    MenuSelection menuSelect2;
    short shootKey;
    short bombKey;
    short focusKey;
    short pauseKey;
    short skipKey;
    short idk25;
    int int_0x59d0;
    int idk26;
    int idk27;
    ReplayManager* replayManager;
    int idk28[99];
    byte* musicCommentFile;
    int g;
    uint32_t moreFlags;
    Thread threadInf;

    static void sub_00459130(MenuSelection* menuSelect);
    static int customizeOptionsMenu(MainMenu* This, uint8_t di);
    static int func3(MainMenu* This);
    static void setMode(MainMenu* This, uint32_t mode);
};
ASSERT_SIZE(MainMenu, 0x5b94);