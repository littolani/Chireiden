#pragma once
#include "Chireiden.h"
#include "Macros.h"

struct Timer
{
public:
    //Timer();
    int m_previous;
    int m_current;
    float m_currentF;
    float* m_gameSpeed;
    uint32_t m_isInitialized;

    static void addf(Timer* This, float amount);

    // 0x459270
    static int increment(Timer* This);

    static void set(Timer* This, int time);

    static BOOL isTickMultipleOf(Timer* This, int i);
};
ASSERT_SIZE(Timer, 0x14);