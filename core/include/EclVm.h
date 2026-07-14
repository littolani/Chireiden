#pragma once
#include "Chireiden.h"
#include "EclManager.h"
#include "EclRunContext.h"
#include "EclStack.h"
#include "Macros.h"

struct EclRunContextList
{
    EclRunContext* entry;
    EclRunContextList* next;
    EclRunContextList* prev;
};

class EclVm
{
public:
     void* vtable; // 0x0
    EclRunContext* currentContext;
    EclRunContext primaryContext;
    void* fileManager;
    EclRunContextList asyncListHead;

    //virtual int __thiscall vfunction1();
    //virtual int __thiscall vfunction2();
    //virtual int __thiscall vfunction3();
    //virtual int __thiscall vfunction4();
    //virtual int __thiscall vfunction5();
};
ASSERT_SIZE(EclVm, 0x103c);