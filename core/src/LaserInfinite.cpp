#include "LaserInfinite.h"

int LaserInfinite::computeCircleAABBCollision(LaserInfinite* This, Float2* targetBoundingCirclePos, float targetCircleRadius)
{
    // Translate target position relative to the laser's base
    float deltaX = targetBoundingCirclePos->x - This->laserBasePosition.x;
    float deltaY = targetBoundingCirclePos->y - This->laserBasePosition.y;

    // Why flip the angle??
    float negAngle = -This->laserAngle;
    float sinA = std::sinf(negAngle);
    float cosA = std::cosf(negAngle);

    float localX = (deltaX * cosA) - (deltaY * sinA);
    float localY = (deltaY * cosA) + (deltaX * sinA);

    // Bounding box dimensions
    float length = This->laserLength;
    float halfWidth = This->laserWidth * 0.5f;

    if (length >= (localX - targetCircleRadius) && 
        (localX + targetCircleRadius) >= 0.0f &&
        (localY - targetCircleRadius) <= halfWidth &&
        (localY + targetCircleRadius) >= -halfWidth) 
    {
        return 2; // Collision detected? FIND OUT WHAT RETURN 1 MEANS
    }

    return 0; // No collision
}   