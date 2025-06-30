#include "AnmVm.h"

void AnmVm::initialize()
{
    D3DXVECTOR3 entityPos = this->entityPos;
    int layer = this->layer;
    memset(this,0,0x434);

    this->scale.x = 1.0;
    this->scale.y = 1.0;
    this->entityPos = entityPos;
    this->layer = layer;
    this->color1 = 0xffffffff;
    D3DXMatrixIdentity(&this->matrix2fc);
    this->flagsLow = 7;
    this->timeInScript.m_current = 0;
    this->timeInScript.m_currentF = 0.0;
    this->timeInScript.m_previous = -999999;
    this->posInterp.endTime = 0;
    this->rgbInterp.endTime = 0;
    this->alphaInterp.endTime = 0;
    this->rotationInterp.endTime = 0;
    this->scaleInterp.endTime = 0;
    this->rgb2Interp.endTime = 0;
    this->alpha2Interp.endTime = 0;
    this->uVelInterp.endTime = 0;
    this->vVelInterp.endTime = 0;
    this->nodeInGlobalList.next = nullptr;
    this->nodeInGlobalList.prev = nullptr;
    this->nodeInGlobalList.entry = this;
    this->nodeAsFamilyMember.next = nullptr;
    this->nodeAsFamilyMember.prev = nullptr;
    this->nodeAsFamilyMember.entry = this;
}

