#include "AnmManager.h"
#include "DebugGui.h"
#include "FpsCounter.h"
#include "Globals.h"
#include "Supervisor.h"
#include "Spellcard.h"
#include "ThunkGenerator.h"
#include "Window.h"

LRESULT Window::wndProcCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lpParam)
{
    switch (uMsg)
    {
    case WM_ACTIVATEAPP:
        g_window.isAppUnfocused = (wParam == FALSE);
        g_window.isAppFocused = wParam;
        break;

    case WM_SIZE:
        if (wParam == SIZE_MAXIMIZED && (g_window.someFlag2 & 1))
        {
            g_window.someFlag2 = (g_window.someFlag2 & ~0xC) | 2;
            return DefWindowProcA(hwnd, WM_SIZE, SIZE_MAXIMIZED, lpParam);
        }
        break;

    case WM_CLOSE:
        g_supervisor.flags = (g_supervisor.flags & ~0x100) | 0x80;
        return 1;

    case WM_ERASEBKGND:
        return 1;

    case WM_SETCURSOR:
        if (g_supervisor.m_d3dPresetParameters.Windowed)
        {
            SetCursor(LoadCursorA(NULL, MAKEINTRESOURCEA(IDC_ARROW)));
            ShowCursor(TRUE);
            return 1;
        }

        // If fullscreen but unfocused (e.g., alt-tabbed), ensure cursor is visible
        if (g_window.isAppUnfocused)
        {
            SetCursor(LoadCursorA(NULL, MAKEINTRESOURCEA(IDC_ARROW)));
            // Force show cursor counter to be >= 0
            while (ShowCursor(TRUE) < 0);
            return 1;
        }

        // If fullscreen and focused, hide the cursor
        // Force show cursor counter to be < 0
        while (ShowCursor(FALSE) >= 0);
        SetCursor(NULL);
        return 1;

    case WM_SYSCOMMAND:
        // Filter out system menu activation to prevent game interruption
        if ((wParam & 0xFFF0) == SC_MOUSEMENU || (wParam & 0xFFF0) == SC_KEYMENU)
            return 1; // Eat the command
        break;

    case WM_LBUTTONDOWN:
        SetForegroundWindow(hwnd);
        break;
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lpParam);
}

int Window::initialize(HINSTANCE hInstance)
{
    WNDCLASSA wc;
    wc.style = 0;
    wc.lpfnWndProc = Window::wndProcCallback; // Using the static wrapper directly
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursorA(NULL, MAKEINTRESOURCEA(IDC_ARROW));
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = TOUHOU_WINDOW_CLASS_NAME;
    RegisterClassA(&wc);

    // Yes, it does seem like 4 options are used to represent two states 
    g_window.isAppFocused = 1;
    g_window.isAppUnfocused = 0;

    uint32_t modeBits = (g_supervisor.m_gameConfig.displayMode << 2);
    g_window.someFlag2 = (g_window.someFlag2 & ~0x0C) | (modeBits & 0x0C);

    BOOL isWindowedMode = (g_window.someFlag2 & 0x0C) != 0;
    g_supervisor.m_d3dPresetParameters.Windowed = isWindowedMode;

    if ((g_supervisor.m_gameConfig.frameSkip == 0) && (g_supervisor.m_gameConfig.latencyMode == 2))
        g_window.someFlag2 |= 0x10;
    else
        g_window.someFlag2 &= ~0x10;

    g_window.idk5 = 15;
    g_window.frameTimingData[0].sleepTimeMs = 15;
    g_window.frameTimingData[0].lagCounter = 0;
    g_window.frameTimingData[0].targetSleepTime = 12;
    g_window.frameTimingData[1].sleepTimeMs = 12;
    g_window.frameTimingData[1].lagCounter = 0;
    g_window.frameTimingData[1].targetSleepTime = 12;
    g_window.frameTimingData[2].sleepTimeMs = 12;
    g_window.frameTimingData[2].lagCounter = 0;
    g_window.frameTimingData[2].targetSleepTime = 8;
    g_window.idk6[0] = 8;
    g_window.idk6[1] = 0;
    g_window.frameTimingDataIndex = 0;

    if (isWindowedMode)
    {
        uint32_t windowScaleMode = (g_window.someFlag2 >> 2) & 3;
        int targetClientWidth;
        int targetClientHeight;

        // Determine the target CLIENT area size
        if (windowScaleMode == 3)
        {
            targetClientWidth = 1280;
            targetClientHeight = 960;
        }
        else if (windowScaleMode == 2)
        {
            targetClientWidth = 960;
            targetClientHeight = 720;
        }
        else
        {
            targetClientWidth = 640;
            targetClientHeight = 480;
        }

        // Use AdjustWindowRect to calculate the correct window dimensions
        // This ensures the client area is exactly the resolution we want, 
        // letting Windows handle the border and title bar calculations.
        RECT rc = { 0, 0, targetClientWidth, targetClientHeight };
        AdjustWindowRect(&rc, WINDOW_STYLE_WINDOWED, FALSE);

        int windowWidth = rc.right - rc.left;
        int windowHeight = rc.bottom - rc.top;

        // We want to create it hidden first, apply Dark Mode, and THEN show it.
        DWORD style = WINDOW_STYLE_WINDOWED & ~WS_VISIBLE;

        g_window.hwnd = CreateWindowExA(
            0,
            TOUHOU_WINDOW_CLASS_NAME,
            TOUHOU_WINDOW_TITLE,
            style,
            g_supervisor.m_gameConfig.windowPosX,
            g_supervisor.m_gameConfig.windowPosY,
            windowWidth,
            windowHeight,
            NULL,
            NULL,
            hInstance,
            NULL
        );
    }
    else
    {
        // Fullscreen Mode
        g_window.hwnd = CreateWindowExA(
            0,
            TOUHOU_WINDOW_CLASS_NAME,
            TOUHOU_WINDOW_TITLE,
            WINDOW_STYLE_FULLSCREEN,
            0,
            0,
            640,
            480,
            NULL,
            NULL,
            hInstance,
            NULL
        );
    }

    if (g_window.hwnd == NULL)
        return 1;

    GetWindowRect(g_window.hwnd, &g_supervisor.windowDimensions);
    g_supervisor.appWindow = g_window.hwnd;

    // Apply Dark Mode Preference BEFORE showing the window
    BOOL enable = TRUE;
    if (FAILED(DwmSetWindowAttribute(g_window.hwnd, 20, &enable, sizeof(enable)))) // DWMWA_USE_IMMERSIVE_DARK_MODE
    {
        DwmSetWindowAttribute(g_window.hwnd, 19, &enable, sizeof(enable)); // Fallback
    }

    // Now show the window. 
    // The original game code uses a Minimize -> Sleep -> Restore cycle.
    // SC_RESTORE will make the window visible if it was created hidden.
    SendMessageA(g_window.hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    Sleep(16);
    SendMessageA(g_window.hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);

    // Safety check: ensure window is actually visible in case SC_RESTORE behavior varies
    if (!IsWindowVisible(g_window.hwnd)) {
        ShowWindow(g_window.hwnd, SW_SHOW);
    }

    return 0;
}

double Window::getDeltaTime()
{
    g_supervisor.enterCriticalSection(5);
    double elapsedTime;
    if (g_window.performanceCounterFreq.QuadPart != 0)
    {
        LARGE_INTEGER currentCounter;
        QueryPerformanceCounter(&currentCounter);
        int64_t elapsedTicks = currentCounter.QuadPart - g_window.performanceCounterFreq.QuadPart;
        elapsedTime = static_cast<double>(elapsedTicks) / g_window.performanceCounterFreq.QuadPart;

        if (elapsedTime < g_window.someDouble)
            g_window.someDouble = elapsedTime;

        elapsedTime -= g_window.someDouble;
    }
    else
    {
        double currentTime = timeGetTime();
        if (g_window.someDouble > currentTime)
            g_window.someDouble = currentTime;
        elapsedTime = (currentTime - g_window.someDouble) / 1000.0;
    }
    g_supervisor.leaveCriticalSection(5);
    return elapsedTime;
}

void Window::frame(Window* This)
{
    if (((g_window.someFlag2 & 0x10) != 0) && (This->deltaTime < This->frameDeltaTime))
    {
        do {
            This->deltaTime += FRAME_RATE_60HZ;
        } while (This->deltaTime < This->frameDeltaTime);
    }
    g_anmManager->flushSprites(g_anmManager);

    // Setup the camera and viewport for the calculation phase
    g_supervisor.currentCam = &g_supervisor.cam1;
    g_supervisor.swapCameraTransformMatrices(&g_supervisor.cam1);
    g_supervisor.d3dDevice->SetViewport(&g_supervisor.currentCam->viewport);
    g_supervisor.camIndex = 1;

    int runCalcChainResult = g_chain->runCalcChain(g_chain);

    // If result is 0 or -1, the game loop should terminate/close.
    if (runCalcChainResult == 0 || runCalcChainResult == -1)
    {
        g_supervisor.thread.close(&g_supervisor.thread);
        return;
    }

    This->frameskipCounter++;
    if (This->frameskipCounter >= (g_supervisor.m_gameConfig.frameSkip + 1))
    {
        g_supervisor.d3dDevice->BeginScene();

        AnmVertexBuffers& buffers = g_anmManager->m_anmVertexBuffers;
        buffers.leftoverSpriteCount = 0;
        buffers.spriteWriteCursor = buffers.spriteVertexData;
        buffers.spriteRenderCursor = buffers.spriteVertexData;

        // Disable Fog for drawing
        g_supervisor.d3dDisableFogFlag = 0xFF;
        g_supervisor.disableD3dFog(&g_supervisor);

        // Execute the Draw Chain (process render commands)
        g_chain->runDrawChain();

        // Flush any sprites added by the Draw Chain
        g_anmManager->flushSprites(g_anmManager);

        g_supervisor.d3dDevice->SetTexture(0, NULL);
        g_supervisor.d3dDevice->EndScene();

        // Reset skip counter and Present the frame
        This->frameskipCounter = 0;
        update(This);
    }

    double currentTime = getDeltaTime();
    g_supervisor.deltaTime = currentTime - This->frameDeltaTime;
}

void Window::update(Window* This)
{
    double currentTime = getDeltaTime();
    This->frameDeltaTime = currentTime;

    // Calculate how much time has passed since the last frame start,
    // and determine how much we need to sleep to hit 60 FPS.
    // Formula: (LastFrameTime + TargetTime - CurrentTime) * 1000 - Bias
    double timeUntilNextFrame = (This->timeSinceLastFrame + TARGET_FRAME_TIME_SEC) - currentTime;
    int calculatedSleepMs = static_cast<int>(timeUntilNextFrame * SEC_TO_MS - SLEEP_BIAS_MS);

    This->sleepTrackerMs = calculatedSleepMs;

    FrameTimingData& currentFrameData = This->frameTimingData[This->frameTimingDataIndex];

    // Adaptive Sleep Logic
    if (currentFrameData.sleepTimeMs < calculatedSleepMs)
    {
        // We slept less than calculated last time? Increment lag counter.
        currentFrameData.lagCounter++;

        // If lag isn't too severe (< 15 frames of lag), try to adjust
        if (currentFrameData.lagCounter < MAX_LAG_FRAMES)
        {
            // Check the previous frame's target sleep time to smooth adjustments
            // Access "previous" in size-3 ring buffer 
            int prevIndex = (This->frameTimingDataIndex + 2) % 3;

            if (currentFrameData.sleepTimeMs < This->frameTimingData[prevIndex].targetSleepTime)
            {
                currentFrameData.sleepTimeMs++;
            }
            // Logic flow continues to clamp check
        }
        else
        {
            // Severe lag: Reset lag counter and proceed
            currentFrameData.lagCounter = 0;
        }
    }
    else
    {
        // Clamp sleep time to 0 minimum
        currentFrameData.sleepTimeMs = (calculatedSleepMs < 0) ? 0 : calculatedSleepMs;

        // Reset lag counter as we are on time
        currentFrameData.lagCounter = 0;
    }

    // Ensure our internal sleep tracker isn't negative
    if (This->sleepTrackerMs < 0)
        This->sleepTrackerMs = 0;

    // VSync / Raster Synchronization
    D3DRASTER_STATUS d3dRasterStatus;
    d3dRasterStatus.ScanLine = 0;

    currentTime = getDeltaTime();
    if (currentTime < This->deltaTime)
        This->deltaTime = currentTime;

    // Calculate time spent waiting so far relative to the sync reference (deltaTime)
    double waitDuration = currentTime - This->deltaTime;

    // Check if we are already lagging beyond the 57 FPS threshold
    if (waitDuration >= LAG_THRESHOLD_SEC)
    {
        // Lag detected immediately, skip VSync wait and reduce sleep time for next frame
        FrameTimingData* frameTimingData = &This->frameTimingData[This->frameTimingDataIndex];
        if (frameTimingData->sleepTimeMs > 0)
            frameTimingData->sleepTimeMs--;
    }
    else
    {
        // We have time--perform precise synchronization
        // Phase 1: Coarse Busy Wait
        // Sleep(1) until we are within the VSync safe period (13ms)
        currentTime = getDeltaTime();
        waitDuration = currentTime - This->deltaTime;

        if (waitDuration < VSYNC_BUSY_WAIT_THRESHOLD_SEC)
        {
            do {
                Sleep(1);
                currentTime = getDeltaTime();
                waitDuration = currentTime - This->deltaTime;
            } while (waitDuration < VSYNC_BUSY_WAIT_THRESHOLD_SEC);
        }

        // Phase 2: Raster Line Polling
        // Get initial scanline to check if we need to wait
        UINT initialScanline = d3dRasterStatus.ScanLine;
        g_supervisor.d3dDevice->GetRasterStatus(0, &d3dRasterStatus);

        if (initialScanline <= d3dRasterStatus.ScanLine)
        {
            // Poll until VBlank or Scanline Wrap
            do {
                // If ScanLine is 0 or InVBlank is true, we hit the vertical blanking interval
                if (d3dRasterStatus.ScanLine == 0 || d3dRasterStatus.InVBlank)
                    break;

                currentTime = getDeltaTime();
                initialScanline = d3dRasterStatus.ScanLine;

                // Watchdog: If we wait too long (drop below 57 FPS), abort to prevent spiraling lag
                if ((currentTime - This->deltaTime) > LAG_THRESHOLD_SEC)
                {
                    FrameTimingData* frameTiming = &This->frameTimingData[This->frameTimingDataIndex];
                    if (frameTiming->sleepTimeMs > 0)
                        frameTiming->sleepTimeMs--;
                    break;
                }

                g_supervisor.d3dDevice->GetRasterStatus(0, &d3dRasterStatus);

            } while (initialScanline <= d3dRasterStatus.ScanLine);
        }
    }

    currentTime = getDeltaTime();
    This->deltaTime = currentTime; // Update sync reference time

    // Take snapshot
    static auto NC_takeSnapshot = createCustomCallingConvention<Storage<void>, Signature<void>>(0x446880);
    NC_takeSnapshot();

    HRESULT hr = g_supervisor.d3dDevice->Present(NULL, NULL, NULL, NULL);

    // Handle Device Loss
    if (hr == D3DERR_DEVICELOST)
    {
        g_supervisor.releaseSurfaces();
        g_anmManager->releaseTextures();

        hr = g_supervisor.d3dDevice->Reset(&g_supervisor.m_d3dPresetParameters);

        g_anmManager->createD3DTextures(g_anmManager);
        Supervisor::setupCameras(&g_supervisor);
        Supervisor::resetRenderState();

        g_supervisor.idk14[2] = 2;
    }

    if (g_fpsCounter)
    {
        static auto fpsCounterRelated2 = createCustomCallingConvention<Storage<void>, Signature<void>>(0x419d90);
        fpsCounterRelated2(); //FpsCounter::fpsCounterRelated2();
    }

    if (g_spellcard)
    {
        static auto NC_spellcardRelated = createCustomCallingConvention<Storage<void>, Signature<void>>(0x40c310);
        NC_spellcardRelated(); //NC_spellcardRelated();
    }


    getDeltaTime(); // Discarded result
    DWORD sleepMs = This->frameTimingData[This->frameTimingDataIndex].sleepTimeMs;
    if ((int)sleepMs > 0)
        Sleep(sleepMs);

    // Store the final frame completion time for the next frame's calculation
    currentTime = getDeltaTime();
    This->timeSinceLastFrame = currentTime;
}


void Window::retrieveSystemStats()
{
    SystemParametersInfoA(0x10, 0, &g_window.screenSaveActive, 0);
    SystemParametersInfoA(0x53, 0, &g_window.lowerPowerActive, 0);
    SystemParametersInfoA(0x54, 0, &g_window.powerOffActive, 0);
    SystemParametersInfoA(0x11, 0, NULL, 2);
    SystemParametersInfoA(0x55, 0, NULL, 2);
    SystemParametersInfoA(0x56, 0, NULL, 2);
    QueryPerformanceFrequency(&g_window.performanceCounterFreq);
    QueryPerformanceCounter(&g_window.performanceCounterValue);
}

void Window::setDarkTitleBar(HWND hwnd)
{

}