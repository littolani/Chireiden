#include "AsciiManager.h"
#include "AnmVm.h"
#include "AnmLoaded.h"
#include "AnmManager.h"
#include "Bullet.h"
#include "Chireiden.h"
#include "DebugGui.h"
#include "EclManager.h"
#include "Globals.h"
#include "ThunkGenerator.h"
#include "FileAbstraction.h"
#include "Player.h"
#include "ReplayManager.h"
#include "ScoreManager.h"
#include "StdVm.h"
#include "SoundManager.h"
#include "LaserLine.h"
#include <conio.h>
#include <winbase.h>

typedef IDirect3D9* (WINAPI* Direct3DCreate9_t)(UINT SDKVersion);
Direct3DCreate9_t Real_Direct3DCreate9 = nullptr;

HMODULE g_hOriginalDll = nullptr;

void LoadOriginalDll()
{
    if (g_hOriginalDll)
        return;

    // Get the path to the System32 folder where the real d3d9.dll lives
    char path[MAX_PATH];
    GetSystemDirectoryA(path, MAX_PATH);
    strcat_s(path, "\\d3d9.dll");

    g_hOriginalDll = LoadLibraryA(path);
    if (!g_hOriginalDll)
    {
        MessageBoxA(NULL, "Could not load system d3d9.dll", "Chireiden Error", MB_ICONERROR);
        ExitProcess(1);
    }

    Real_Direct3DCreate9 = (Direct3DCreate9_t)GetProcAddress(g_hOriginalDll, "Direct3DCreate9");
}

extern "C" IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion) {
    LoadOriginalDll();

    // Eventually hook IDirect3D9::CreateDevice to initialize ImGui later.
    
    return Real_Direct3DCreate9(SDKVersion);
}

void logfThunk(const char* format, ...)
{
    // Get current time
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_s(&tm_now, &now);

    // Print timestamp
    char timeBuf[32];
    strftime(timeBuf, sizeof(timeBuf), "[%Y-%m-%d %H:%M:%S] ", &tm_now);
    fputs(timeBuf, stdout);

    // Print formatted message
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void logThunk(const char* str)
{
    logfThunk("%s", str);
}

void setupConsole()
{
    if (!AllocConsole())
        return;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize;
    bufferSize.X = 4096;
    bufferSize.Y = 2000;
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // Redirect stdout, stdin, and stderr
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    // Enable ANSI Colors
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // Sync C++ streams (std::cout, std::cin, etc.)
    std::ios::sync_with_stdio(true);

    SetConsoleTitleA("Reimu's Debug Console");
}

void waitForDebugger()
{
    if (!IsDebuggerPresent()) {
        printf("Waiting for debugger to attach to th11.exe (or press any key to skip)...\n");

        while (!IsDebuggerPresent())
        {
            if (_kbhit())
            {
                _getch();
                printf("Key pressed. Continuing without debugger.\n");
                return;
            }
            Sleep(100);
        }

        printf("Debugger attached!\n");
        Sleep(500);
    }
}

void snapshot()
{
    const char* a = reinterpret_cast<char*>(0x4c9928);
    puts(a);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        setupConsole();

        installHook(0x445510, createLtoThunk<EAX, Stack<0x4>, Stack<0x8>, Stack<0xc>, Stack<0x10>>(WinMain, 0));

        ///installHook(0x4540f0, createLtoThunk<Void, ESI>(AnmLoadedD3D::createTextureFromAtR, 0));

        //installHook(0x455a70, createLtoThunk<Void, Stack<0x4>, Stack<0x8>, Stack<0xc>, Stack<0x10>, EAX>(AsciiManager::spawnAnm, 0x10));

        //installHook(0x4014e0, createLtoThunk<Void, ESI, ECX, EBX>(AsciiManager::loadAsciiStrings, 0));

        //installHook(0x4526f0, createLtoThunk<EAX, Stack<0x4>>(AnmManager::initialize, 0x4));

#if 1
        installHook(0x44fd10, createLtoThunk<Void, ESI>(AnmManager::flushSprites, 0));
        installHook(0x44f710, createLtoThunk<Void, EAX, EDI>(AnmManager::setupRenderStateForVm, 0));
        installHook(0x44f4b0, createLtoThunk<Void, EAX, Stack<0x4>>(AnmManager::applyRenderStateForVm, 0x4));
        installHook(0x44fda0, createLtoThunk<Void, EAX, EDX>(AnmManager::writeSprite, 0));
        installHook(0x44f880, createLtoThunk<Void, ECX, Stack<0x4>, EAX>(AnmManager::drawVmSprite2D, 0x4));
        installHook(0x4543e0, createLtoThunk<EAX, ECX, Stack<0x4>, EBX>(AnmManager::openAnmLoaded, 0x4));
        installHook(0x454190, createLtoThunk<EAX, Stack<0x4>, Stack<0x8>, ECX>(AnmManager::preloadAnmFromMemory, 0x8));
        installHook(0x454360, createLtoThunk<EAX, ECX, EBX>(AnmManager::preloadAnm, 0));
        installHook(0x450e20, createLtoThunk<EAX, Stack<0x4>, EAX>(AnmManager::updateWorldMatrixAndProjectQuadCorners, 0x4));
        installHook(0x4513a0, createLtoThunk<EAX, Stack<0x4>, EBX>(AnmManager::drawVmWithTextureTransform, 0x4));
        installHook(0x450b00, createLtoThunk<EAX, EDI, ESI>(AnmManager::drawVmWithFog, 0));
        installHook(0x451ef0, createLtoThunk<Void, ECX, EAX>(AnmManager::drawVm, 0));
        installHook(0x453220, AnmManager::setupRenderSquare);
        installHook(0x4569c0, AnmManager::allocateVm);

        // Already hooked through internal calls
        //installHook(0x4503d0, createLtoThunk<Void, EAX, Stack<0x4>, EBX, EDI, ESI>(AnmVm::applyZRotationToQuadCorners, 0x4));
        //installHook(0x4500f0, createLtoThunk<Void, Stack<0x4>, EBX, EDI, EDX, ESI>(AnmVm::writeSpriteCharacters, 0x4));
        //installHook(0x450700, createLtoThunk<EAX, EAX>(AnmVm::projectQuadCornersThroughCameraViewport, 0));
        

        //installHook(0x452420, createLtoThunk<EAX, ECX>(AnmVm::onTick, 0));

        installHook(0x40a280, createLtoThunk<Void, EDX>(Bullet::step_ex_00_speedup, 0));
        installHook(0x40a300, createLtoThunk<Void, EAX>(Bullet::step_ex_02_accel, 0));

        installHook(0x45b220, createLtoThunk<EAX, ESI, EBX, EDI, EAX>(CWaveFile::open, 0));
        installHook(0x45b480, createLtoThunk<EAX, ESI, Stack<0x4>, Stack<0x8>, EAX>(CWaveFile::read, 0x8));
        installHook(0x45b3a0, createLtoThunk<EAX, ESI, BL, EDI>(CWaveFile::resetFile, 0));
        installHook(0x45b2d0, createLtoThunk<EAX, ECX, EAX, Stack<0x4>>(CWaveFile::reopen, 0x4));

        //installHook(0x449660, createLtoThunk<EAX, ECX, Stack<0x4>, EAX, EDI>(SoundManager::findRiffChunk, 0x4));
        installHook(0x44a1e0, createLtoThunk<Void, ESI>(SoundManager::playSoundCentered, 0));
        installHook(0x44a260, createLtoThunk<Void, Stack<0x4>, EDI>(SoundManager::playSoundWithPan, 0x4));

#if 0
        // This replaces the game's function with my own implementation for testing purposes
        static LPVOID playSoundWithPanShimTrampoline = createTrampoline(0x44a260, 5);

        static auto playSoundWithPanShim = [](float xOffsetFromCenter, int soundId) -> void
        {
            printf("Playing sound %d at x = %.2f\n", soundId, xOffsetFromCenter);

            // let's change the sounds for fun so we know the shim worked
            soundId -= 1;
            if (soundId < 0) soundId = 0;

            using ltoSig = Storage<Void, Stack<0x4>, EDI>;
            using ltoFunc = Signature<Void, float, int>;
            static auto playSoundWithPanOriginal = createCustomCallingConvention<ltoSig, ltoFunc>((uintptr_t)playSoundWithPanShimTrampoline);
            playSoundWithPanOriginal(xOffsetFromCenter, soundId);
        };
        installTrampoline(0x44a260, createLtoThunk<Void, Stack<0x4>, EDI>(playSoundWithPanShim, 0x4), 5);
#endif

        installHook(0x457080, createLtoThunk<Void, EDX, ECX>(Chain::cut, 0));
        installHook(0x456cb0, createLtoThunk<EAX, EBX>(Chain::runCalcChain, 0));
        installHook(0x456e10, createLtoThunk<EAX>(Chain::runDrawChain, 0));
        installHook(0x456c10, createLtoThunk<EAX, ESI, EBX>(Chain::registerDrawChain, 0));
        installHook(0x456b70, createLtoThunk<EAX, ESI, EBX>(Chain::registerCalcChain, 0));
        installHook(0x45d900, createLtoThunk<EAX, ECX, Stack<0x4>>(EclManager::parse, 0x4));
        installHook(0x410d50, createLtoThunk<EAX, ECX, Stack<0x4>>(EclManager::loadInclude, 0x4));
        installHook(0x4104d0, createLtoThunk<EAX, ECX, Stack<0x4>>(EclManager::cacheAndParse, 0x4));

        // installHook(0x426df0, createLtoThunk<Returns<EAX, ECX, Stack<0x4>, Stack<0x8>>(LaserLine::createOrDestroyLaserSegments, 8));

        installHook(0x441760, createLtoThunk<EAX, Stack<0x4>, Stack<0x8>, ECX>(PbgArchive::readDecompressEntry, 0x8));
        installHook(0x4418d0, createLtoThunk<EAX, EAX, EBX>(PbgArchive::findEntry, 0));
        installHook(0x4426c0, createLtoThunk<EAX, Stack<0x4>, Stack<0x8>, Stack<0xc>, EAX>(Lzss::decompress, 0xc));
        installHook(0x4423a0, createLtoThunk<EAX, Stack<0x4>, Stack<0x8>, Stack<0xc>>(Lzss::compress, 0xc));
        installHook(0x4581c0, createLtoThunk<Void, Stack<0x4>, Stack<0x8>, AL, Stack<0xc>, Stack<0x10>, Stack<0x14>>(FileUtils::decrypt, 0x14));
  
        installHook(0x437500, createLtoThunk<EAX, ECX, Stack<0x4>>(ScoreManager::calculateChecksum, 0x4));

        installHook(0x403950, createLtoThunk<EAX, EAX, EDX, Stack<0x4>, ECX>(StdVm::checkEntityVisibility, 0x4));

        installHook(0x437300, createLtoThunk<EAX, EBX, EDI>(ReplayManager::appendReplayChunk, 0));

        installHook(0x446d30, createLtoThunk<EAX, EDI>(Supervisor::initD3d9Devices, 0));

        installHook(0x459270, createLtoThunk<EAX, ESI>(Timer::increment, 0));

        //installHook(0x446ae0, createLtoThunk<EAX, EBX>(Window::initialize, 0));

        //installHook(0x430290, createLtoThunk<Void, Stack<4>>(Player::move, 4));

        installHook(0x432cc0, createLtoThunk<Void, EBX>(Player::repopulateAbilities, 0));
        //installHook(0x4336a0, createLtoThunk<Void, ESI>(Player::abilityDrawCallbackReimuA, 0));

        // installHook(0x4461f0, createLtoThunk<Void, ESI>(Window::update, 0));
#endif
        // Globals
        installHook(0x458400, createLtoThunk<EAX, EAX, Stack<0x4>, Stack<0x8>>(openFile, 0x8));
        installHook(0x40b9d0, createLtoThunk<Void, ECX, Stack<0x4>, Stack<0x8>>(projectMagnitudeToVectorComponents, 0x8));
        installHook(0x459bc0, createLtoThunk<Void, Stack<0x4>, Stack<0x8>>(logfThunk, 0));
        installHook(0x441c80, createLtoThunk<Void, Stack<0x4>, Stack<0x8>>(logfThunk, 0));
        installHook(0x44ab00, createLtoThunk<Void, Stack<0x4>, Stack<0x8>>(logfThunk, 0));
        installHook(0x456ad0, createLtoThunk<Void, Stack<0x4>, Stack<0x8>>(logfThunk, 0));
        installHook(0x45b5a0, createLtoThunk<Void, Stack<0x4>, Stack<0x8>>(logfThunk, 0));
        installHook(0x41f6e0, createLtoThunk<Void, Stack<0x4>, Stack<0x8>>(logfThunk, 0));
        installHook(0x429ca0, snapshot);
        installHook(0x458a10, createLtoThunk<Void, Stack<0x4>>(logThunk, 0));
        installHook(0x458af0, createLtoThunk<Void, Stack<0x4>>(logThunk, 0));

        waitForDebugger();
    }
    return TRUE;
}