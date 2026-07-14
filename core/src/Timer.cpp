#include "Timer.h"
#include "Globals.h"

// 0x406100
void Timer::set(Timer* This, int time)
{
    if ((This->m_isInitialized & 1U) == 0)
    {
        This->m_currentF = 0.0;
        This->m_current = 0;
        This->m_previous = -999999;
        This->m_gameSpeed = &g_gameSpeed;
        This->m_isInitialized |= 1;
    }
    This->m_current = time;
    This->m_previous = time - 1;
    This->m_currentF = time;
}

void Timer::addf(Timer* This, float amount)
{
    This->m_previous = This->m_current;
    float gameSpeed = *This->m_gameSpeed;
    if (0.99f < gameSpeed && gameSpeed < 1.01f)
    {
        This->m_currentF += amount;
        This->m_current = static_cast<int>(This->m_currentF);
        return;
    }
    This->m_currentF = gameSpeed * amount + This->m_currentF;
    This->m_current = static_cast<int>(This->m_currentF);
}

int Timer::increment(Timer* This)
{
    float gameSpeed = *This->m_gameSpeed;
    This->m_previous = This->m_current;
    if (0.99f < gameSpeed && gameSpeed < 1.01f)
    {
        This->m_current += 1;
        This->m_currentF += 1.0f;
        return This->m_current;
    }
  This->m_currentF += gameSpeed;
  This->m_current = static_cast<int>(This->m_currentF);
  return This->m_current;
}

BOOL Timer::isTickMultipleOf(Timer* This, int i)
{
    if ((This->m_current != This->m_previous) && (This->m_current % i == 0))
        return TRUE;
    return FALSE;
}

