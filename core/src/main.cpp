#include "AsciiManager.h"
#include "AnmVm.h"
#include "AnmLoaded.h"
#include "AnmManager.h"
#include "Bullet.h"
#include "Chireiden.h"
#include "DebugGui.h"
#include "Globals.h"
#include "ThunkGenerator.h"
#include "FileAbstraction.h"
#include "ScoreManager.h"
#include "SoundManager.h"
#include <winbase.h>

void resolveLnkShortcut(LPCSTR shortcutPath, LPSTR targetPath)
{
    if (targetPath == nullptr)
        return;

    // Initialize COM
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr))
        return;

    IShellLinkA* shellLink = nullptr;
    IPersistFile* persistFile = nullptr;
    WCHAR* wideShortcutPath = nullptr;

    // Set targetPath to empty string initially to indicate failure if not filled
    targetPath[0] = '\0';

    // Create IShellLinkA instance
    hr = CoCreateInstance(
        CLSID_ShellLink,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IShellLinkA,
        (void**)&shellLink
    );
    if (FAILED(hr))
        goto Cleanup;

    // Query for IPersistFile interface
    hr = shellLink->QueryInterface(IID_IPersistFile, (void**)&persistFile);
    if (FAILED(hr))
        goto Cleanup;

    // Allocate memory for wide-character shortcut path
    wideShortcutPath = new WCHAR[260];
    if (wideShortcutPath == nullptr)
        goto Cleanup;

    // Convert shortcutPath to wide-character string
    MultiByteToWideChar(
        CP_ACP,
        0,
        shortcutPath,
        -1,
        wideShortcutPath, 260);

    // Load the shortcut file
    hr = persistFile->Load(wideShortcutPath, 0);
    if (SUCCEEDED(hr)) {
        // Get the target path
        shellLink->GetPath(targetPath, 260, nullptr, 0);
    }

Cleanup:
    if (wideShortcutPath)
        delete[] wideShortcutPath;
    if (persistFile)
        persistFile->Release();
    if (shellLink)
        shellLink->Release();
    CoUninitialize();
}

int getLaunchInfo()
{
    STARTUPINFOA startupInfo;
    char resolvedPath[264];
    char moduleFilename[268];
    char* fileExtensionString;

    startupInfo.cb = 0x44;
    memset(&startupInfo.lpReserved, 0, 0x40);
    GetModuleFileNameA(nullptr, moduleFilename, 0x105);
    GetConsoleTitleA(resolvedPath, 0x105);
    GetStartupInfoA(&startupInfo);

    if (startupInfo.lpTitle == nullptr)
        g_supervisor.flags |= 0x40;
    else
    {
        fileExtensionString = strrchr(startupInfo.lpTitle, '.');
        if (fileExists(startupInfo.lpTitle) && fileExtensionString != nullptr)
        {
            if (_stricmp(fileExtensionString, ".lnk") == 0)
            {
                do {
                    resolveLnkShortcut(startupInfo.lpTitle, resolvedPath);
                    fileExtensionString = strrchr(resolvedPath, '.');
                } while (_stricmp(fileExtensionString, ".lnk") == 0);
            }
            else
                strcpy_s(resolvedPath, startupInfo.lpTitle);

            if (strcmp(resolvedPath, moduleFilename) != 0)
                g_window.unusualLaunchFlag = 1;
        }
        g_supervisor.flags &= ~0x40;
    }
    return (g_app != nullptr) ? 0 : -1;
}

// Define Control IDs for readability
#define IDC_CHK_ALWAYS_SHOW 0xCA
#define IDC_RAD_FS_640      0xCC
#define IDC_RAD_WIN_640     0xCD
#define IDC_RAD_WIN_960     0xCE
#define IDC_RAD_WIN_1280    0xCF
#define IDC_BTN_START       0xD0

// Flag definitions for g_window.someFlag2
#define WND_FLAG_DIALOG_CANCELLED 0x20
#define WND_FLAG_DIALOG_ACTIVE    0x40
#define WND_FLAG_DIALOG_MASK      0x60

BOOL CALLBACK chooseResolutionDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        // Setup Checkbox state based on config
        // Flag 0x100 determines if this dialog is shown on startup
        UINT checkState = (g_supervisor.m_gameConfig.flags & 0x100) ? BST_CHECKED : BST_UNCHECKED;
        CheckDlgButton(hDlg, IDC_CHK_ALWAYS_SHOW, checkState);

        // Select the correct Radio Button based on current display mode
        int radioIdToSelect = IDC_RAD_FS_640;
        switch (g_supervisor.m_gameConfig.displayMode)
        {
        case 0: radioIdToSelect = IDC_RAD_FS_640; break;
        case 1: radioIdToSelect = IDC_RAD_WIN_640; break;
        case 2: radioIdToSelect = IDC_RAD_WIN_960; break;
        case 3: radioIdToSelect = IDC_RAD_WIN_1280; break;
        default: break;
        }
        CheckRadioButton(hDlg, IDC_RAD_FS_640, IDC_RAD_WIN_1280, radioIdToSelect);

        // Update Window State Flag
        // Clear 0x20, Set 0x40 (Mark dialog as Active)
        g_window.someFlag2 = (g_window.someFlag2 & ~WND_FLAG_DIALOG_CANCELLED) | WND_FLAG_DIALOG_ACTIVE;

        return TRUE;
    }

    case WM_COMMAND:
    {
        // Check if the source is the "Start" button (0xD0)
        if (LOWORD(wParam) == IDC_BTN_START)
        {
            // Read Checkbox
            if (IsDlgButtonChecked(hDlg, IDC_CHK_ALWAYS_SHOW) == BST_CHECKED)
                g_supervisor.m_gameConfig.flags |= 0x100;
            else
                g_supervisor.m_gameConfig.flags &= ~0x100;

            // Read Radio Buttons to set display mode
            if (IsDlgButtonChecked(hDlg, IDC_RAD_FS_640) == BST_CHECKED)
                g_supervisor.m_gameConfig.displayMode = 0;
            else if (IsDlgButtonChecked(hDlg, IDC_RAD_WIN_640) == BST_CHECKED)
                g_supervisor.m_gameConfig.displayMode = 1;
            else if (IsDlgButtonChecked(hDlg, IDC_RAD_WIN_960) == BST_CHECKED)
                g_supervisor.m_gameConfig.displayMode = 2;
            else
                g_supervisor.m_gameConfig.displayMode = 3;

            // Clear Dialog Flags (Success path)
            g_window.someFlag2 &= ~WND_FLAG_DIALOG_MASK;

            EndDialog(hDlg, 6);
            return TRUE;
        }
        break;
    }

    case WM_CLOSE: // 0x10
        if ((g_window.someFlag2 & WND_FLAG_DIALOG_MASK) == WND_FLAG_DIALOG_ACTIVE)
            g_window.someFlag2 = (g_window.someFlag2 & ~WND_FLAG_DIALOG_ACTIVE) | WND_FLAG_DIALOG_CANCELLED;

        EndDialog(hDlg, 6);
        return TRUE;
    }

    return FALSE;
}

int checkJoystickAvailability(void)
{
    MMRESULT joystickStatus;
    joyinfoex_tag joystickInfo;

    joystickInfo.dwSize = 0x34;
    joystickInfo.dwFlags = 0xff;
    joystickStatus = joyGetPosEx(0, &joystickInfo);
    if (joystickStatus != 0) {
        joystickStatus = joyGetPosEx(1, &joystickInfo);
        if (joystickStatus != 0) {
            printf("使えるパッドが存在しないようです、残念\n");
            return 1;
        }
    }
    joyGetDevCapsA(0, &g_joyCaps, 0x194);
    return 0;
}

void normalizeKeyboardState(void)
{
    int keyIndex;
    byte keyboardState[256];

    GetKeyboardState(keyboardState);
    keyIndex = 0;
    do {
        keyboardState[keyIndex] = keyboardState[keyIndex] & 0x7f;
        keyIndex = keyIndex + 1;
    } while (keyIndex < 256);
    SetKeyboardState(keyboardState);
}


// 0x445510
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    g_window.hInstance = hInstance;
    timeBeginPeriod(1);

    g_supervisor.flags |= 0x8000;
    for (int i = 0; i < 12; ++i)
    {
        InitializeCriticalSection(&g_supervisor.criticalSections[i]);
    }

    puts("---------- Touhou 11 Startup Log ----------\n");

    g_app = CreateMutexA(NULL, TRUE, "Touhou 11 App");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxA(NULL, "二つは起動できません\n", "エラー", MB_OK | MB_ICONERROR);
        goto AppCleanup;
    }

    if (getLaunchInfo() == -1)
        goto AppCleanup;

    g_supervisor.hInstance = hInstance;
    g_window.retrieveSystemStats();

    if (g_supervisor.verifyGameConfig() == 0)
    {
        BYTE keyboardBuffer[256];
        GetKeyboardState(keyboardBuffer);

        // Show config dialog if configured to do so, or if Left Ctrl (0x10) is pressed?
        if ((g_supervisor.m_gameConfig.flags & 0x100) != 0 || (keyboardBuffer[VK_LCONTROL] & 0x80) != 0)
            DialogBoxParamA(hInstance, (LPCSTR)0xCB, NULL, chooseResolutionDialog, 0);

        if ((g_window.someFlag2 & WND_FLAG_DIALOG_MASK) == 0)
            g_window.someFlag2 ^= ((g_supervisor.m_gameConfig.displayMode * 4) ^ g_window.someFlag2) & 0xC;
    }

RestartEngine:
    while (true)
    {
        g_soundManager.someState = 2;
        g_soundManager.close();
        g_soundManager.releaseSounds();

        if (g_anmManager)
        {
            g_anmManager->~AnmManager();
            game_free(g_anmManager);
            g_anmManager = nullptr;
        }

        if (g_supervisor.surfaceR0) { g_supervisor.surfaceR0->Release(); g_supervisor.surfaceR0 = nullptr; }
        if (g_supervisor.surfaceR1) { g_supervisor.surfaceR1->Release(); g_supervisor.surfaceR1 = nullptr; }
        if (g_supervisor.backBuffer) { g_supervisor.backBuffer->Release(); g_supervisor.backBuffer = nullptr; }
        if (g_supervisor.d3dDevice) { g_supervisor.d3dDevice->Release(); g_supervisor.d3dDevice = nullptr; }
        if (g_supervisor.d3dInterface0) { g_supervisor.d3dInterface0->Release(); g_supervisor.d3dInterface0 = nullptr; }

        if (g_window.hwnd)
        {
            ShowWindow(g_window.hwnd, SW_HIDE);
            MoveWindow(g_window.hwnd, 0, 0, 0, 0, 0);
            DestroyWindow(g_window.hwnd);
            g_window.hwnd = nullptr;
        }

        while (ShowCursor(TRUE) < 0);

        // Exit check
        if (g_window.timeForCleanup != 2)
        {
            writeToFile("th11.cfg", sizeof(GameConfig), &g_supervisor.m_gameConfig);
            timeEndPeriod(1);

            g_supervisor.flags &= ~0x8000;
            for (int i = 0; i < 12; ++i)
                DeleteCriticalSection(&g_supervisor.criticalSections[i]);

            SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, g_window.screenSaveActive, NULL, SPIF_SENDCHANGE);
            SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, g_window.lowerPowerActive, NULL, SPIF_SENDCHANGE);
            SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, g_window.powerOffActive, NULL, SPIF_SENDCHANGE);
            WINNLSEnableIME(NULL, TRUE);
            return 0;
        }

        puts("再起動を要するオプションが変更されたので再起動します\n");

        if (!g_supervisor.m_d3dPresetParameters.Windowed)
            WINNLSEnableIME(NULL, TRUE);

        // Flush message queue before restart
        tagMSG msg;
        for (int i = 0; i < 60; ++i)
        {
            if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
        }
        g_supervisor.flags &= ~0x180;

        // Allocation and Init
        void* chainMem = game_new(sizeof(Chain));
        g_chain = chainMem ? new (chainMem) Chain() : nullptr;

        checkJoystickAvailability();
        normalizeKeyboardState();

        g_supervisor.flags &= ~0xC00;
        Supervisor::initializeInputDevices(&g_supervisor);

        uint32_t ioFlags = g_supervisor.flags ^ ((uint32_t)(g_supervisor.keyboard) << 10 ^ g_supervisor.flags) & 0x400;
        g_supervisor.flags = ioFlags ^ ((uint32_t)(g_supervisor.joystick) << 11 ^ ioFlags) & 0x800;

        g_supervisor.d3dInterface0 = Direct3DCreate9(D3D_SDK_VERSION);
        if (!g_supervisor.d3dInterface0)
        {
            puts("Direct3D オブジェクトは何故か作成出来なかった\r\n");
            break;
        }

        if (!g_window.initialize(hInstance))
            break;

        g_soundManager.createThread(g_window.hwnd);

        if (g_supervisor.initD3d9Devices(D3DFMT_UNKNOWN) != 0)
            continue;

        void* anmMem = game_malloc(sizeof(AnmManager));
        if (!anmMem)
        {
            printf("Failed to allocate AnmManager!\n");
            exit(1);
        }
        g_anmManager = AnmManager::initialize((AnmManager*)anmMem);

        if (!g_supervisor.m_d3dPresetParameters.Windowed)
        {
            WINNLSEnableIME(NULL, FALSE);
            while (ShowCursor(FALSE) >= 0);
            SetCursor(NULL);
        }

        g_window.someDouble = 0.0;
        g_window.frameDeltaTime = g_window.getDeltaTime();
        g_window.frameSkipDeltaTime = g_window.frameDeltaTime;
        g_window.predictedDeltaTime = g_window.frameDeltaTime;
        g_window.timeSinceLastFrame = g_window.getDeltaTime();
        g_window.deltaTime = g_window.timeSinceLastFrame;

        SetForegroundWindow(g_window.hwnd);
        g_supervisor.initialize();

        g_window.someFlag2 |= 1;
        g_window.timeForCleanup = 0;
        g_window.frameskipCounter = -4;

    GameLoop:
        while (true)
        {
            if (g_window.timeForCleanup != 0)
                goto EngineCleanup;

            if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
                continue;
            }

            HRESULT hr = g_supervisor.d3dDevice->TestCooperativeLevel();
            if (SUCCEEDED(hr))
            {
                if ((g_window.someFlag2 & 2) == 0)
                {
                    if ((g_window.someFlag2 & 0x10) == 0)
                    {
                        if (g_supervisor.m_d3dPresetParameters.PresentationInterval == 1 && g_supervisor.m_gameConfig.frameSkip == 0) {
                            // Window::frameIdkWhatVariationThisIs(&g_window);
                        }
                        else {
                            // Window::frameFrameskip(&g_window);
                        }
                    }
                    else
                        Window::frame(&g_window);

                    if (g_window.timeForCleanup != 0)
                        break;

                    g_supervisor.flags &= ~0x10;
                    continue;
                }
            }
            else if (hr != D3DERR_DEVICENOTRESET)
            {
                continue;
            }

            // Device Reset Handling
            g_window.idk2 = 10;
            if ((g_window.someFlag2 & 2) != 0)
            {
                if ((g_window.someFlag2 & 0xC) == 0)
                {
                    GetWindowRect(g_window.hwnd, &g_supervisor.windowDimensions);
                    g_supervisor.m_d3dPresetParameters.PresentationInterval = ((g_window.someFlag2 & 0x10) ? 0 : D3DPRESENT_INTERVAL_ONE);
                    g_supervisor.m_d3dPresetParameters.FullScreen_RefreshRateInHz = 60;
                    g_supervisor.m_d3dPresetParameters.Windowed = FALSE;
                    g_supervisor.m_d3dPresetParameters.BackBufferFormat = (g_supervisor.m_gameConfig.colorDepth != 0) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
                }
                else
                {
                    g_supervisor.m_d3dPresetParameters.FullScreen_RefreshRateInHz = 0;
                    g_supervisor.m_d3dPresetParameters.PresentationInterval = (g_window.someFlag2 & 0x10) ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_ONE;
                    g_supervisor.m_d3dPresetParameters.Windowed = TRUE;
                }
            }

            g_supervisor.releaseSurfaces();
            g_anmManager->releaseTextures();

            hr = g_supervisor.d3dDevice->Reset(&g_supervisor.m_d3dPresetParameters);
            if (SUCCEEDED(hr))
            {
                g_supervisor.resetRenderState();
                g_anmManager->createD3DTextures(g_anmManager);
                g_supervisor.flags |= 0x10;
                g_supervisor.idk7[3] = 3;

                if ((g_window.someFlag2 & 2) != 0)
                {
                    uint32_t displayMode = (g_window.someFlag2 >> 2) & 3;
                    if (displayMode == 0)
                    {
                        SetWindowLongA(g_window.hwnd, GWL_STYLE, WS_POPUP);
                        SetWindowPos(g_window.hwnd, HWND_TOP, 0, 0, 640, 480, SWP_FRAMECHANGED);
                        WINNLSEnableIME(NULL, FALSE);
                        while (ShowCursor(FALSE) >= 0);
                        SetCursor(NULL);
                        g_window.isAppUnfocused = 0;
                    }
                    else
                    {
                        int width, height;
                        if (displayMode == 3) {
                            width = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 1280;
                            height = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 960;
                        }
                        else if (displayMode == 2) {
                            width = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 960;
                            height = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 720;
                        }
                        else {
                            width = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 640;
                            height = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 480;
                        }
                        int captionHeight = GetSystemMetrics(SM_CYCAPTION);

                        SetWindowLongA(g_window.hwnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
                        SetWindowPos(
                            g_window.hwnd, HWND_TOP,
                            g_supervisor.windowDimensions.left, g_supervisor.windowDimensions.top,
                            width, height + captionHeight,
                            SWP_FRAMECHANGED | SWP_SHOWWINDOW
                        );

                        ShowWindow(g_window.hwnd, SW_SHOW);
                        WINNLSEnableIME(NULL, TRUE);
                        while (ShowCursor(TRUE) < 0);
                    }
                }
                g_supervisor.setupCameras(&g_supervisor);
                g_window.someFlag2 &= ~2; // Clear device lost flag
            }
        }

    EngineCleanup:
        g_supervisor.m_gameConfig.displayMode = (g_window.someFlag2 >> 2) & 3;
        if (g_supervisor.m_gameConfig.displayMode != 0)
        {
            GetWindowRect(g_window.hwnd, &g_supervisor.windowDimensions);
            g_supervisor.m_gameConfig.windowPosX = g_supervisor.windowDimensions.left;
            g_supervisor.m_gameConfig.windowPosY = g_supervisor.windowDimensions.top;
        }

        if (g_chain)
        {
            Supervisor::releaseChains(); // or g_chain->release();
            game_free(g_chain);
            g_chain = nullptr;
        }

        goto RestartEngine;
    }

AppCleanup:
    return 0;
}
