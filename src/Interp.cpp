#include "Interp.h"

template <typename T>
T Interp<T>::step()
{

}

template <>
float Interp<float>::interpolate()
{
    float t = m_timer.m_currentF / m_endTime;

    switch(m_method)
    {
    case InterpMethod::In2:
        return pow2(t);
    case InterpMethod::In3:
        return pow3(t);
    case InterpMethod::In4:
        return pow4(t);
    case InterpMethod::Out2:
        return 1.0f - pow2(1.0f - t);
    case InterpMethod::Out3:
        return 1.0f - pow3(1.0f - t);
    case InterpMethod::Out4:
        return 1.0f - pow4(1.0f - t);
    case InterpMethod::InOut2:
        t *= 2.0f;
        if (t < 1.0f) return pow2(t) / 2.0f;
        else return (2.0f - pow2(2.0f - t)) / 2.0f;
    case InterpMethod::InOut3:
        t *= 2.0f;
        if (t < 1.0f) return pow3(t) / 2.0f;
        else return (2.0f - pow3(2.0f - t)) / 2.0f;
    case InterpMethod::InOut4:
        t *= 2.0f;
        if (t < 1.0f) return pow4(t) / 2.0f;
        else return (2.0f - pow4(2.0f - t)) / 2.0f;
    case InterpMethod::OutIn2:
        t *= 2.0f;
        if (t < 1.0f) return 0.5f - pow2(1.0f - t) / 2.0f;
        else return 0.5f + pow2(t - 1.0f) / 2.0f;
    case InterpMethod::OutIn3:
        t *= 2.0f;
        if (t < 1.0f) return 0.5f - pow3(1.0f - t) / 2.0f;
        else return 0.5f + pow3(t - 1.0f) / 2.0f;
    case InterpMethod::OutIn4:
        t *= 2.0f;
        if (t < 1.0f) return 0.5f - pow4(1.0f - t) / 2.0f;
        else return 0.5f + pow4(t - 1.0f) / 2.0f;
    case InterpMethod::Delayed:
        return 0.0f;
    case InterpMethod::Instant:
        return 1.0f;
    default:
        return t;
    }
}
