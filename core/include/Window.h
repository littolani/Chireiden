#pragma once
#include "Chireiden.h"
#include "Macros.h"

struct FrameTimingData
{
    int sleepTimeMs;
    int lagCounter;
    int targetSleepTime;
};

#pragma pack(push, 1)
class Window
{
public:
    HWND hwnd;
    DWORD timeForCleanup;
    HINSTANCE hInstance;

    // Yes, it does seem like 4 options are used to represent two states
    BOOL isAppFocused;
    BOOL isAppUnfocused;

    char frameskipCounter;
    char idk0[3];
    LARGE_INTEGER performanceCounterFreq;
    LARGE_INTEGER performanceCounterValue;
    DWORD unusualLaunchFlag;
    DWORD screenSaveActive;
    DWORD lowerPowerActive;
    int powerOffActive;
    uint32_t someFlag2;
    int idk2;
    double frameDeltaTime;
    double frameSkipDeltaTime;
    double predictedDeltaTime;
    double someDouble;
    double timeSinceLastFrame;
    double deltaTime;
    int idk4;
    int sleepTrackerMs;
    int frameTimingDataIndex;
    int idk5;
    FrameTimingData frameTimingData[3];
    int idk6[2];

    // 0x445e00
    static LRESULT CALLBACK wndProcCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lpParam);

    // 0x446ae0
    static int initialize(HINSTANCE hInstance);

    static int frame(Window* This);
    static int frameFrameskip(Window* This);
    static int frameIdkWhatVariationThisIs(Window* This);
    static void update(Window* This);
    static double getDeltaTime();
    static void retrieveSystemStats();
private:
    static void setDarkTitleBar(HWND hwnd);

    static constexpr LPCSTR TOUHOU_WINDOW_CLASS_NAME = "BASE";
    static constexpr LPCSTR TOUHOU_WINDOW_TITLE = "Subterranean Animism";
    static constexpr DWORD WINDOW_STYLE_WINDOWED = (WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
    static constexpr DWORD WINDOW_STYLE_FULLSCREEN = (WS_POPUP | WS_VISIBLE);
    static constexpr double TARGET_FRAME_TIME_SEC = 1.0 / 60.0;
    static constexpr double LAG_THRESHOLD_SEC = 1.0 / 57.0;
    static constexpr double VSYNC_BUSY_WAIT_THRESHOLD_SEC = 0.013;
    static constexpr double SLEEP_BIAS_MS = 2.5;
    static constexpr double SEC_TO_MS = 1000.0;
    static constexpr int MAX_LAG_FRAMES = 15;
    static constexpr double FRAME_RATE_60HZ = 1.0 / 60.0;
};
#pragma pack(pop)
ASSERT_SIZE(Window, 0xac);