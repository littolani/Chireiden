#pragma once
#include "AnmVm.h"
#include "Chireiden.h"
#include "Macros.h"

class Gui
{
public:
    uint32_t unk[4];
    AnmVm vms[16];
    int idk1[58];
    int playerRelatedValue;
    int idk2[7];
};
ASSERT_SIZE(Gui, 0x4458);
