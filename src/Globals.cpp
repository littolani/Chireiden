#include "Globals.h"

Supervisor g_supervisor{};
Chain* g_chain{};
HANDLE g_app{};
DWORD g_unusualLaunchFlag{};
HINSTANCE g_hInstance{};
DWORD g_primaryScreenWorkingArea{};
DWORD g_mouseSpeed{};
DWORD g_screenWorkingArea{};
LARGE_INTEGER g_performanceFrequency{};
LARGE_INTEGER g_performanceCount{};
PbgArchive g_pbgArchive{};
D3DFORMAT g_d3dFormats[] = { D3DFMT_UNKNOWN, D3DFMT_A8R8G8B8, D3DFMT_A1R5G5B5, D3DFMT_R5G6B5, D3DFMT_R8G8B8, D3DFMT_A4R4G4B4 };
uint32_t g_bytesPerPixelLookupTable[] = { 4, 4, 2, 2, 3, 2, 0, 1, 2};
double g_time = 0.0;

double getDeltaTime()
{
    g_supervisor.enterCriticalSection(5);
    double elapsedTime;
    if (g_performanceFrequency.QuadPart != 0)
    {
        LARGE_INTEGER currentCounter;
        QueryPerformanceCounter(&currentCounter);
        int64_t elapsedTicks = currentCounter.QuadPart - g_performanceCount.QuadPart;
        elapsedTime = static_cast<double>(elapsedTicks) / g_performanceFrequency.QuadPart;

        if (elapsedTime < g_time) {
            g_time = elapsedTime;
        }
        elapsedTime -= g_time;
    } 
    else
    {
        double currentTime = timeGetTime();
        if (currentTime < 0)
            currentTime += 4294967296.0;
        if (g_time > currentTime)
            g_time = currentTime;
        elapsedTime = (currentTime - g_time) / 1000.0;
    }
    g_supervisor.leaveCriticalSection(5);
    return elapsedTime;
}