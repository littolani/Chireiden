#include "Enemy.h"

EclRunContext* Enemy::getEclRunContextViaAsyncId(Enemy* This, int asyncId)
{
    EclRunContextList* listNode = &This->eclVm.asyncListHead;
    while (listNode)
    {
        if (listNode->entry->asyncId == asyncId)
            return listNode->entry;
        listNode = listNode->next;
    }
    return nullptr;
}
