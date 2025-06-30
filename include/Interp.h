#pragma once
#include "Chireiden.h"
#include "Timer.h"

enum class InterpMethod
{
    In2 = 0,
    In3 = 1,
    In4 = 2,
    Out2 = 3,
    Out3 = 4,
    Out4 = 5,
    Unhandled7 = 6,
    Unhandled8 = 7,
    InOut2 = 8,
    InOut3 = 9,
    InOut4 = 10,
    OutIn2 = 11,
    OutIn3 = 12,
    OutIn4 = 13,
    Delayed = 14,
    Instant = 15,
    Unhandled17 = 16
};

template <typename T>
struct Interp 
{
    T m_initial;
    T m_goal;
    T m_bezier1;
    T m_bezier2;
    Timer m_timer;
    int m_endTime;
    InterpMethod m_method;
    
    constexpr T pow2(T t) { return t * t; }
    constexpr T pow3(T t) { return t * t * t; }
    constexpr T pow4(T t) { return t * t * t * t; }

    T step();
    T interpolate();
};