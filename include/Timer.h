#pragma once

#include "Chireiden.h"
#include "Globals.h"

struct Timer
{
    int m_previous;
    int m_current;
    float m_currentF;
    float* m_gameSpeed;
    uint32_t m_isInitialized;

    void addf(float amount);
    void setCurrent(int time);
    void increment();
};
