#include "Chireiden.h"
#include "Chain.h"
#include "Globals.h"

#define MAX_PATH_LEN 260

int fileExists(LPCSTR filePath)
{
    HANDLE fileHandle;

    g_supervisor.enterCriticalSection(2);

    fileHandle = CreateFileA(
        filePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(fileHandle);
        g_supervisor.leaveCriticalSection(2);
        return true;
    }

    g_supervisor.leaveCriticalSection(2);
    return false;
}

void resolveLnkShortcut(LPCSTR shortcutPath, LPSTR targetPath)
{
    if (targetPath == NULL)
        return;

    // Initialize COM
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return;

    IShellLinkA *shellLink = NULL;
    IPersistFile *persistFile = NULL;
    WCHAR *wideShortcutPath = NULL;

    // Set targetPath to empty string initially to indicate failure if not filled
    targetPath[0] = '\0';

    // Create IShellLinkA instance
    hr = CoCreateInstance(
        CLSID_ShellLink,
        NULL,
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
    wideShortcutPath = new WCHAR[MAX_PATH_LEN];
    if (wideShortcutPath == NULL)
        goto Cleanup;

    // Convert shortcutPath to wide-character string
    MultiByteToWideChar(
        CP_ACP,
        0,
        shortcutPath,
        -1,
        wideShortcutPath, MAX_PATH_LEN);

    // Load the shortcut file
    hr = persistFile->Load(wideShortcutPath, 0);
    if (SUCCEEDED(hr)) {
        // Get the target path
        shellLink->GetPath(targetPath, MAX_PATH_LEN, NULL, 0);
    }

Cleanup:
    if (wideShortcutPath) free(wideShortcutPath);
    if (persistFile) persistFile->Release();
    if (shellLink) shellLink->Release();
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
    GetModuleFileNameA(NULL, moduleFilename, 0x105);
    GetConsoleTitleA(resolvedPath, 0x105);
    GetStartupInfoA(&startupInfo);

    if (startupInfo.lpTitle == NULL)
        g_supervisor.criticalSectionFlag |= 0x40;
    else
    {
        fileExtensionString = strrchr(startupInfo.lpTitle, '.');
        if (fileExists(startupInfo.lpTitle) && fileExtensionString != NULL)
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
                g_unusualLaunchFlag = 1;
        }
        g_supervisor.criticalSectionFlag &= ~0x40;
    }
    return (g_app != NULL) ? 0 : -1;
}

void retrieveSystemStats()
{
  SystemParametersInfoA(0x10, 0, &g_primaryScreenWorkingArea, 0);
  SystemParametersInfoA(0x53, 0, &g_mouseSpeed, 0);
  SystemParametersInfoA(0x54, 0, &g_screenWorkingArea, 0);
  SystemParametersInfoA(0x11, 0, NULL, 2);
  SystemParametersInfoA(0x55, 0, NULL, 2);
  SystemParametersInfoA(0x56, 0, NULL, 2);
  QueryPerformanceFrequency(&g_performanceFrequency);
  QueryPerformanceCounter(&g_performanceCount);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    HINSTANCE thisHInstance = hInstance;
    g_hInstance = hInstance;

    // Initialize the critical sections
    g_supervisor.criticalSectionFlag = g_supervisor.criticalSectionFlag | 0x8000;
    for (size_t j = 0; j < 12; ++j)
        InitializeCriticalSection(&g_supervisor.criticalSections[j]);

    printf("---東方動作記録---\n");
    g_app = CreateMutexA(NULL, 1, "Touhou 11 App");

    DWORD error = GetLastError();
    if (error == 0xb7)
        printf("二つは起動できません\n");

    else
    {
        int launchInfo = getLaunchInfo();
        g_supervisor.hInstance = thisHInstance;
        if (launchInfo != -1)
        {
            g_supervisor.hInstance = thisHInstance;
            retrieveSystemStats();

            initStatus = NC_processGameConfig(&g_supervisor);
            if (initStatus == 0)
            {
                GetKeyboardState(keyboardBuffer);
                if (((g_supervisor.gameCfg._56_4_ & 0x100) != 0) || ((bStack_100 & 0x80) != 0)) {
                    DialogBoxParamA(hinst,(LPCSTR)0xcb,(HWND)0x0,dialogBoxProcedure,0);
            }
            if ((g_windowConfigFlags & 0x60) == 0) 
            {
                g_windowConfigFlags = g_windowConfigFlags ^
               ((uint)g_supervisor.gameCfg.mysteryField * 4 ^ g_windowConfigFlags) & 0xc;
                FN_calculateSelfChecksum();
                d3dformat = D3DFMT_UNKNOWN;
                goto InitChain;
            }
        }
    }

InitChain:
    g_chain = new Chain;
    if (!g_chain)
    {
        printf("Failed to allocate chain\n");
        exit(1);
    }

    g_chain->calcChain.jobRunDrawChainCallback = nullptr;
    g_chain->calcChain.registerChainCallback = nullptr;
    g_chain->calcChain.runCalcChainCallback = nullptr;
    g_chain->calcChain.jobNode = nullptr;
    g_chain->calcChain.jobTrackerNode = &g_chain->calcChain;
    g_chain->calcChain.jobNextTrackerChain = nullptr;
    g_chain->calcChain.jobPreviousTrackerNode = nullptr;

    g_chain->drawChain.jobRunDrawChainCallback = nullptr;
    g_chain->drawChain.registerChainCallback = nullptr;
    g_chain->drawChain.runCalcChainCallback = nullptr;
    g_chain->drawChain.jobNode = nullptr;
    g_chain->drawChain.jobTrackerNode = &g_chain->drawChain;
    g_chain->drawChain.jobNextTrackerChain = nullptr;
    g_chain->drawChain.jobPreviousTrackerNode = nullptr;
    g_chain->timeToRemove = 0;

    ChainElem** calcChainNextElem = &(g_chain->calcChain).nextNode;
    *calcChainNextElem = (ChainElem*)(-2);
    calcChainNextElem = &(g_chain->drawChain).nextNode;
    *calcChainNextElem = (ChainElem*)(-2);

    std::cout << "Press Enter to exit...\n";
    std::cin.get();

    if (g_chain != nullptr) {
        g_chain->release();
        free(g_chain);
    }
    g_chain = nullptr;

    return 0;
}
