#pragma once
#include "Chireiden.h"
#include "Macros.h"
#include "LaserBase.h"

class LaserInfinite : public LaserBase
{
public:
    

    static int computeCircleAABBCollision(LaserInfinite* This, Float2* targetPos, float radius);
};