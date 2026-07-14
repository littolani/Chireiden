#pragma once

class AnmVm;
struct AnmVmListNode
{
    AnmVm* entry;
    AnmVmListNode* next;
    AnmVmListNode* prev;
};