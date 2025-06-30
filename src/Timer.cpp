#include "Timer.h"

void Timer::addf(float amount)
{
    m_previous = m_current;
    float gameSpeed = *m_gameSpeed;
    if (0.99f < gameSpeed && gameSpeed < 1.01f)
    {
        m_currentF += amount;
        m_current = static_cast<int>(m_currentF);
        return;
    }
    m_currentF = gameSpeed * amount + m_currentF;
    m_current = static_cast<int>(m_currentF);
}

void Timer::increment()
{
    float gameSpeed = *m_gameSpeed;
    m_previous = m_current;
    if (0.99f < gameSpeed && gameSpeed < 1.01f)
    {
        m_current += 1;
        m_currentF += 1.0f;
        return;
    }
  m_currentF += gameSpeed;
  m_current = static_cast<int>(m_currentF);
}

void Timer::setCurrent(int time)
{
    if ((m_isInitialized & 1U) == 0)
    {
        m_currentF = 0.0;
        m_current = 0;
        m_previous = -999999;
        m_gameSpeed = &g_gameSpeed;
        m_isInitialized |= 1;
    }
    m_current = time;
    m_previous = time - 1;
    m_currentF = time;
    return;
}


