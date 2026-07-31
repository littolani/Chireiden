#include "Supervisor.h"
#include "AnmManager.h"
#include "AnmLoaded.h"
#include "Chain.h"
#include "Globals.h"
#include "Window.h"
#include "GameConfig.h"
#include "DebugGui.h"
#include "InputManager.h"
#include <bit>

int Supervisor::initialize()
{
    g_supervisor.gameMode = -2;
    g_supervisor.gameModeToSwitchTo = 0;
    g_supervisor.idk6 = 0;

    ChainElem* chainElem = (ChainElem*)game_new(sizeof(ChainElem));
    if (!chainElem)
    {
        puts("Supervisor::initialize(): Could not allocate ChainElem\n");
        return -1;
    }

    auto addChain = [&](int priority, ChainCallback callback, ChainCallback onRegisterCb, bool isCalcChain) -> int
    {
        ChainElem* elem = (ChainElem*)game_new(sizeof(ChainElem));
        elem->nextNode = (ChainElem*)((uintptr_t)elem->nextNode | 3);
        elem->jobRunDrawChainCallback = callback;
        elem->args = &g_supervisor;
        elem->registerChainCallback = nullptr;
        elem->runCalcChainCallback = nullptr;

        int res = 0;
        if (isCalcChain)
            res = g_chain->registerCalcChain(elem, priority);
        else
            res = g_chain->registerDrawChain(elem, priority);
        return res;
    };

    addChain(1, onTick, onRegister, true);
    addChain(1, onDraw01, nullptr, false);
    addChain(0xb, onDraw0b, nullptr, false);
    addChain(0xd, onDraw0dVm0AndLayer27, nullptr, false);
    addChain(0x23, onDraw23, nullptr, false);
    addChain(0x25, onDraw25Vm1AndLayer28, nullptr, false);
    addChain(0x2e, onDraw2e, nullptr, false);
    addChain(0x2f, onDraw2fVm2, nullptr, false);
    addChain(0x44, onDraw44, nullptr, false);
    return 0;
}

ChainCallbackResult Supervisor::onTick(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onRegister(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw01(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw0b(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw0dVm0AndLayer27(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw23(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw25Vm1AndLayer28(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw2e(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw2fVm2(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult Supervisor::onDraw44(void* This)
{
    Supervisor* sup = reinterpret_cast<Supervisor*>(This);
    return ChainCallbackResult::Continue;
}

void Supervisor::releaseDinputIface()
{
    IDirectInput8A* iDirectInput8;
    iDirectInput8 = dInputInterface;
    if (iDirectInput8)
    {
        iDirectInput8->Release();
        dInputInterface = nullptr;
    }
}

void Supervisor::enterCriticalSection(size_t criticalSectionNumber)
{
    if (criticalSectionNumber >= 12)
        return;

    if ((flags & 0x8000) != 0)
    {
        EnterCriticalSection(&criticalSections[criticalSectionNumber]);
        ++criticalSectionCounters[criticalSectionNumber];
    }
}

void Supervisor::leaveCriticalSection(size_t criticalSectionNumber)
{
    if (criticalSectionNumber >= 12)
        return;

    if ((flags & 0x8000) != 0)
    {
        LeaveCriticalSection(&criticalSections[criticalSectionNumber]);
        --criticalSectionCounters[criticalSectionNumber];
    }
}

// 0x429eb0
int Supervisor::verifyGameConfig()
{
    size_t fileSize;
    byte* configFile = openFile("th11.cfg", &fileSize, 1);
    m_gameConfig = GameConfig();

    bool isValid = true;

    if (configFile == nullptr)
    {
        printf("Config file could not be found\n");
        isValid = false;
    }
    else
    {
        memcpy(&m_gameConfig, configFile, sizeof(GameConfig));
        game_free(configFile);

        if (m_gameConfig.colorDepth >= 2 ||
            m_gameConfig.sfxEnabled >= 3 ||
            m_gameConfig.startingBombs >= 2 ||
            m_gameConfig.displayMode >= 4 ||
            m_gameConfig.frameSkip >= 3 ||
            m_gameConfig.musicMode >= 3 ||
            m_gameConfig.version != 0x110003 ||
            fileSize != sizeof(GameConfig))
        {
            printf("Config file is invalid\n");
            isValid = false;
        }
    }

    if (!isValid)
        m_gameConfig = GameConfig(); // Reinitialize
    else
    {
        g_defaultGameConfig.shootKey = m_gameConfig.shootKey;
        g_defaultGameConfig.bombKey = m_gameConfig.bombKey;
        g_defaultGameConfig.focusKey = m_gameConfig.focusKey;
        g_defaultGameConfig.pauseKey = m_gameConfig.pauseKey;
        g_defaultGameConfig.upKey = m_gameConfig.upKey;
        g_defaultGameConfig.downKey = m_gameConfig.downKey;
        g_defaultGameConfig.leftKey = m_gameConfig.leftKey;
        g_defaultGameConfig.rightKey = m_gameConfig.rightKey;
        g_defaultGameConfig.skipKey = m_gameConfig.skipKey;
    }

    uint32_t flags = m_gameConfig.flags;

    if (flags & 0x1)
        printf("Using 16-bit textures.\n");

    if (m_d3dPresetParameters.Windowed != 0)
        printf("Starting in windowed mode.\n");

    if (flags & 0x2)
        printf("Force the reference rasterizer.\n");

    if (flags & 0x4)
        printf("Not using fog.\n");

    if (flags & 0x8)
        printf("DirectInput is not used for gamepad and keyboard input.\n");

    if (flags & 0x10)
        printf("Loading BGM into memory\n");

    m_noVerticalSyncFlag = 0;
    if (flags & 0x20) {
        printf("Vertical synchronization is not enabled.\n");
        m_noVerticalSyncFlag = 1;
    }

    if (flags & 0x40)
        printf("The text rendering environment is not automatically detected.\n");

    int writeStatus = writeToFile("th11.cfg", sizeof(GameConfig), &m_gameConfig);
    if (writeStatus != 0)
    {
        printf("Could not write file %s\n", "th11.cfg");
        printf("Is the disk full or write-protected?\n");
        return -1;
    }
    return 0;
}

// 0x447270
void Supervisor::resetRenderState()
{
    g_supervisor.d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // Render both sides of triangles
    g_supervisor.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x01);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0xFFA0A0A0); // A:255, R:160, G:160, B:160
    g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGDENSITY, std::bit_cast<DWORD>(1.0f));
    g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGSTART, std::bit_cast<DWORD>(1000.0f));
    g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGEND, std::bit_cast<DWORD>(5000.0f));
    g_supervisor.d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
    g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);   // No Mipmap filtering
    g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); // Linear Magnification
    g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); // Linear Minification
    g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

    if (g_anmManager)
    {
        g_anmManager->m_renderStateMode = 7;
        g_anmManager->m_unk_4355c1 = -1;
        g_anmManager->m_haveFlushedSprites = -1;
        g_anmManager->m_anmLoadedD3D = nullptr;
        g_anmManager->m_unk_4355c4 = -1;
    }
    return;
}

BOOL CALLBACK enumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCEA lpddoi, LPVOID pvRef)
{
    if ((lpddoi->dwType & DIDFT_AXIS) != 0) {
        DIPROPRANGE diprg;

        diprg.diph.dwSize = sizeof(DIPROPRANGE); // 0x18
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); // 0x10
        diprg.diph.dwObj = lpddoi->dwType;      // The Object ID (Type)
        diprg.diph.dwHow = DIPH_BYID;           // 2 = By Identifier
        diprg.lMin = -1000;               // 0xFFFFFC18
        diprg.lMax = 1000;                // 0x000003E8

        if (FAILED(g_supervisor.joystick->SetProperty(DIPROP_RANGE, &diprg.diph))) {
            return DIENUM_STOP;
        }
    }
    return DIENUM_CONTINUE;
}

// 0x00447cb0
BOOL CALLBACK enumJoysticksCallback(LPCDIDEVICEINSTANCEA lpddi, LPVOID pvRef)
{
    if (g_supervisor.joystick == nullptr)
    {
        HRESULT hr = g_supervisor.dInputInterface->CreateDevice(
            lpddi->guidInstance,
            &g_supervisor.joystick,
            nullptr
        );

        if (FAILED(hr))
            return DIENUM_CONTINUE;
    }
    return DIENUM_STOP;
}

int Supervisor::initializeInputDevices(Supervisor* This)
{
    if (This->m_gameConfig.flags & 8)
        return -1;

    HRESULT hr = DirectInput8Create(
        g_window.hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&This->dInputInterface,
        nullptr
    );

    if (FAILED(hr))
    {
        This->dInputInterface = nullptr;
        puts("DirectInput cannot be used\n");
        return -1;
    }

    hr = This->dInputInterface->CreateDevice(GUID_SysKeyboard, &This->keyboard, nullptr);

    if (FAILED(hr))
    {
        if (This->dInputInterface) {
            This->dInputInterface->Release();
            This->dInputInterface = nullptr;
        }
        puts("Could not initialize DirectInput\n");
        return -1;
    }

    hr = This->keyboard->SetDataFormat(&c_dfDIKeyboard);

    if (FAILED(hr))
    {
        if (This->keyboard) {
            This->keyboard->Release();
            This->keyboard = nullptr;
        }

        if (This->dInputInterface) {
            This->dInputInterface->Release();
            This->dInputInterface = nullptr;
        }
        puts("DirectInput SetDataFormat failed\n");
        return -1;
    }

    hr = This->keyboard->SetCooperativeLevel(
        This->appWindow,
        DISCL_NOWINKEY | DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
    );

    if (FAILED(hr))
    {
        if (This->keyboard) {
            This->keyboard->Release();
            This->keyboard = nullptr;
        }

        if (This->dInputInterface) {
            This->dInputInterface->Release();
            This->dInputInterface = nullptr;
        }
        puts("DirectInput SetCooperativeLevel Failed\n");
        return -1;
    }

    This->keyboard->Acquire();
    puts("DirectInput Initialized\n");

    This->dInputInterface->EnumDevices(
        DI8DEVCLASS_GAMECTRL,
        enumJoysticksCallback,
        nullptr,
        DIEDFL_ATTACHEDONLY
    );

    if (This->joystick != nullptr)
    {
        This->joystick->SetDataFormat(&c_dfDIJoystick);
        This->joystick->SetCooperativeLevel(
            This->appWindow,
            DISCL_BACKGROUND | DISCL_NONEXCLUSIVE
        );

        This->controllerCaps->dwSize = 0x2C;
        This->joystick->GetCapabilities(This->controllerCaps);
        This->joystick->EnumObjects(enumDeviceObjectsCallback, nullptr, DIDFT_ALL);
        
        puts("Found a valid gamepad\n");
    }
    return 0;
}

void Supervisor::renderFrameWithReset()
{
    g_supervisor.d3dDevice->Clear(0, nullptr, 3, 0xff000000, 1.0, 0);
    HRESULT hr = g_supervisor.d3dDevice->Present(
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    if (FAILED(hr))
        g_supervisor.d3dDevice->Reset(&g_supervisor.m_d3dPresetParameters);

    g_supervisor.d3dDevice->Clear(
        0,
        nullptr,
        3,
        0xff000000,
        1.0,
        0
    );
    hr = g_supervisor.d3dDevice->Present(
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );
    if (FAILED(hr))
        g_supervisor.d3dDevice->Reset(&g_supervisor.m_d3dPresetParameters);
}

void Supervisor::setupViewport()
{
    if (g_anmManager)
        AnmManager::flushSprites(g_anmManager);
   
    g_supervisor.d3dViewport.MinZ = 0.0;
    g_supervisor.d3dViewport.X = 0;
    g_supervisor.d3dViewport.Y = 0;
    g_supervisor.d3dViewport.MaxZ = 1.0;
    g_supervisor.d3dViewport.Width = 640;
    g_supervisor.d3dViewport.Height = 480;
    g_supervisor.d3dDevice->SetViewport(&g_supervisor.d3dViewport);
    renderFrameWithReset();
}

// 0x446d30
int Supervisor::initD3d9Devices(D3DFORMAT d3dFormat)
{
    D3DDISPLAYMODE currentDisplayMode;
    D3DPRESENT_PARAMETERS d3dpp;
    HRESULT hr;
    Supervisor* This = &g_supervisor;

    // Zero out presentation parameters (memset at 446d46)
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));

    // Get the current display mode (446d5c)
    This->d3dInterface0->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &currentDisplayMode);

    // Cache the adapter details (446d6d - 446d82)
    This->currentDisplayModeWidth = currentDisplayMode.Width;
    This->currentDisplayModeHeight = currentDisplayMode.Height;
    This->d3dPresentationIntervalFlag = currentDisplayMode.RefreshRate;
    This->d3dPresentBackBuferFormat = currentDisplayMode.Format;

    // config displayMode check (fullscreen vs windowed flag handling)
    if (This->m_gameConfig.displayMode != 0)
    {
        if (currentDisplayMode.RefreshRate != 60)
        {
            puts("リフレッシュレートが60Hzではありません\n");
            g_window.someFlag2 &= ~0x10; // Remove the fullscreen/windowed bit
        }
    }

    // Determine Color Depth / BackBufferFormat
    if ((This->m_gameConfig.flags & 1) != 0)
    {
        // Windowed Mode: Force 16-bit color depth equivalent
        d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
        This->m_gameConfig.colorDepth = 1;
    }
    else
    {
        // Fullscreen Mode
        if (This->m_gameConfig.colorDepth == 0xFF)
        {
            puts("初回起動、画面を 32Bits で初期化しました\n");
            d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
            This->m_gameConfig.colorDepth = 0;
        }
        else
        {
            d3dpp.BackBufferFormat = (This->m_gameConfig.colorDepth == 0) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
        }
    }

    // Handling unusual launch and VSync settings
    if (g_window.unusualLaunchFlag == 0)
    {
        if (This->m_noVerticalSyncFlag == 0)
        {
            if ((g_window.someFlag2 & 0x10) == 0)
            {
                // Standard Fullscreen
                d3dpp.FullScreen_RefreshRateInHz = 60;
                d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
            }
            else
            {
                // Standard Windowed
                d3dpp.FullScreen_RefreshRateInHz = 0;
                d3dpp.PresentationInterval = (This->m_gameConfig.latencyMode == 3) ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_ONE;
            }
            puts("リフレッシュレートを60Hzに変更を試みます\r\n");
            d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        }
        else
        {
            // VSync disabled in config
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            puts("VSync非同期可能かどうかを試みます\r\n");
            d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        }
    }
    else
    {
        // Safe mode / Unusual Launch: Force Windowed and Immediate VSync (446ebb block)
        This->m_noVerticalSyncFlag = 1;
        d3dpp.BackBufferFormat = currentDisplayMode.Format;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.Windowed = TRUE;

        if (This->m_gameConfig.latencyMode == 3 || currentDisplayMode.RefreshRate == 60) {
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        }
        else {
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        }
    }

    This->flags |= 2;
    d3dpp.BackBufferWidth = 640;
    d3dpp.BackBufferHeight = 480;
    if (g_window.unusualLaunchFlag == 0) d3dpp.Windowed = ((g_window.someFlag2 & 0x10) != 0); // Setup Windowed if not previously forced
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER; // 1

    This->idk8 = 1; // VSync sync capable flag
    bool isResetAttempted = false;

    // Retry loop for device creation
    while (true)
    {
        // Attempt HAL if config doesn't force REF
        if ((This->m_gameConfig.flags & 2) == 0)
        {
            // Try Hardware Vertex Processing
            hr = This->d3dInterface0->CreateDevice(
                D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_window.hwnd,
                D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &This->d3dDevice);

            if (SUCCEEDED(hr)) {
                puts("T&L HAL で動作しま〜す\r\n");
                This->flags |= 1;
                break;
            }

            if (isResetAttempted) {
                puts("T&L HAL は使用できないようです\r\n");
            }

            // Try Software Vertex Processing
            hr = This->d3dInterface0->CreateDevice(
                D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_window.hwnd,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &This->d3dDevice);

            if (SUCCEEDED(hr)) {
                puts("HAL で動作します\r\n");
                This->flags &= ~1;
                break;
            }

            if (isResetAttempted) {
                puts("HAL も使用できないようです\r\n");
        }
    }

        // Fallback to REF rasterizer
        hr = This->d3dInterface0->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_window.hwnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &This->d3dDevice);

        if (SUCCEEDED(hr)) {
            puts("REF で動作しますが、重すぎて恐らくゲームになりません...\r\n");
            This->flags &= ~1;
            break;
        }

        // Handle Failure
        if (This->m_noVerticalSyncFlag == 0) {
            puts("リフレッシュレートが変更できません\r\n");
            d3dpp.FullScreen_RefreshRateInHz = 0; // Drop explicit refresh rate
            This->idk8 = 0;
            isResetAttempted = true;
        }
        else {
            if (d3dpp.PresentationInterval == D3DPRESENT_INTERVAL_ONE) {
                d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            }
            else {
                puts("Direct3D の初期化に失敗、これではゲームは出来ません\r\n");
                if (This->d3dInterface0 != nullptr) {
                    This->d3dInterface0->Release();
                    This->d3dInterface0 = nullptr;
                }
                return 1;
            }
        }
}

    // Save final creation parameters
    memcpy(&This->m_d3dPresetParameters, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));

    // Setup matrices
    float fov = 0.5235988f; // ~30 degrees in radians
    D3DXVECTOR3 cameraPosition(320.0f, -240.0f, -(240.0f / tanf(fov)));
    D3DXVECTOR3 cameraTargetPoint(320.0f, -240.0f, 0.0f);
    D3DXVECTOR3 cameraUpVector(0.0f, 1.0f, 0.0f);

    D3DXMatrixLookAtLH(&This->d3dMatrix1, &cameraPosition, &cameraTargetPoint, &cameraUpVector);
    D3DXMatrixPerspectiveFovLH(&This->d3dMatrix2, fov, 1.3333334f /* 640/480 */, 10.0f, 10000.0f);

    This->d3dDevice->SetTransform(D3DTS_VIEW, &This->d3dMatrix1);
    This->d3dDevice->SetTransform(D3DTS_PROJECTION, &This->d3dMatrix2);
    This->d3dDevice->GetViewport(&This->d3dViewport);

    // Capabilities check
    This->d3dDevice->GetDeviceCaps(&This->d3dcaps);

    if ((This->d3dcaps.TextureOpCaps & D3DTEXOPCAPS_ADD) == 0) {
        puts("D3DTEXOPCAPS_ADD をサポートしていません、色加算エミュレートモードで動作します\r\n");
    }

    if (This->d3dcaps.MaxTextureWidth < 512) {
        puts("512 以上のテクスチャをサポートしていません。殆どの絵がボケて表示されます。\r\n");
    }

    // Alpha support check
    if ((This->m_gameConfig.flags & 1) == 0 && (d3dFormat >> 24) != 0)
    {
        if (This->d3dInterface0->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, currentDisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8) == D3D_OK)
        {
            This->flags |= 4; // Supports alpha format
        }
        else
        {
            This->flags &= ~4;
            This->m_gameConfig.flags |= 1; // Force fallback
            puts("D3DFMT_A8R8G8B8 をサポートしていません、減色モードで動作します\r\n");
        }
    }

    This->resetRenderState();
    This->setupViewport();

    g_window.timeForCleanup = 0;
    This->idk24 = 0;

    return 0;
}

void Supervisor::setupCameras(Supervisor* This)
{
    if (This->surfaceR0 == nullptr)
    {
        IDirect3DTexture9* tex = This->textAnm->m_anmLoadedD3D[2].m_texture; // ??
        tex->GetSurfaceLevel(0, &This->surfaceR0);
        This->arcadeVm0->loadIntoAnmVm(This->arcadeVm0, This->textAnm, 0x51);
        if (This->surfaceR0 == nullptr)
            memcpy(&This->stageCam, &This->cam0, sizeof(Camera));
    
        tex = This->textAnm->m_anmLoadedD3D[3].m_texture;
        tex->GetSurfaceLevel(0, &This->surfaceR1);
        This->arcadeVm1->loadIntoAnmVm(This->arcadeVm1, This->textAnm, 0x52);
        This->arcadeVm2->loadIntoAnmVm(This->arcadeVm2, This->textAnm, 0x51);
        This->d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &This->backBuffer);
    }
}

void Supervisor::swapCameraTransformMatrices(Camera* cam)
{
    // Touhou 11 uses a fixed FOV of PI/10 (approx 18 degrees) for This camera mode
    const float FOV_RADIANS = D3DX_PI / 10.0f;
    const float Z_NEAR = 1.0f;
    const float Z_FAR = 10000.0f;

    if (g_anmManager)
        AnmManager::flushSprites(g_anmManager);

    float viewportW = static_cast<float>(static_cast<uint32_t>(cam->viewport.Width));
    float viewportH = static_cast<float>(static_cast<uint32_t>(cam->viewport.Height));

    float halfW = viewportW * 0.5f;
    float halfH = viewportH * 0.5f;

    // Calculate Eye Z position
    // The camera is positioned so that the viewport height aligns exactly with the FOV at z=0.
    // Formula: distance = (height / 2) / tan(fov / 2)
    float tanHalfFov = std::tan(FOV_RADIANS * 0.5f);
    float eyeZ = halfH / tanHalfFov;

    // Setup Camera Vectors
    D3DXVECTOR3 eyeVec(halfW, halfH, eyeZ);
    D3DXVECTOR3 atVec(halfW, halfH, 0.0f);
    D3DXVECTOR3 upVec(0.0f, -1.0f, 0.0f);

    // Build View Matrix
    D3DXMatrixLookAtLH(&cam->viewMatrix, &eyeVec, &atVec, &upVec);

    // Build Projection Matrix
    D3DXMatrixPerspectiveFovLH(
        &cam->projectionMatrix,
        FOV_RADIANS,
        viewportW / viewportH,
        Z_NEAR,
        Z_FAR
    );

    // Apply Transforms
    if (g_supervisor.d3dDevice)
    {
        g_supervisor.d3dDevice->SetTransform(D3DTS_VIEW, &cam->viewMatrix);
        g_supervisor.d3dDevice->SetTransform(D3DTS_PROJECTION, &cam->projectionMatrix);
    }

    // Apply Global Offsets (likely camera shake or scrolling)
    if (g_anmManager)
    {
        g_anmManager->m_globalRenderQuadOffsetX = cam->m_globalRenderQuadOffsetX;
        g_anmManager->m_globalRenderQuadOffsetY = cam->m_globalRenderQuadOffsetY;
    }
}

void Supervisor::releaseSurfaces()
{
    if (g_supervisor.surfaceR0)
    {
        g_supervisor.surfaceR0->Release();
        g_supervisor.surfaceR0 = NULL;
    }
    if (g_supervisor.surfaceR1)
    {
        g_supervisor.surfaceR1->Release();
        g_supervisor.surfaceR1 = NULL;
    }
    if (g_supervisor.backBuffer)
    {
        g_supervisor.backBuffer->Release();
        g_supervisor.backBuffer = NULL;
    }
    g_supervisor.surfaceR0 = NULL;
    return;
}

void Supervisor::releaseChains()
{
    g_supervisor.thread.close(&g_supervisor.thread);
    g_chain->timeToRemove = 1;
    g_chain->runCalcChain(g_chain);
    g_chain->releaseSingleChain(g_chain, &g_chain->calcChain);
    g_chain->releaseSingleChain(g_chain, &g_chain->drawChain);
    g_chain->drawChain.jobRunDrawChainCallback = nullptr;
    g_chain->drawChain.registerChainCallback = nullptr;
    g_chain->drawChain.runCalcChainCallback = nullptr;
    g_chain->calcChain.jobRunDrawChainCallback = nullptr;
    g_chain->calcChain.registerChainCallback = nullptr;
    g_chain->calcChain.runCalcChainCallback = nullptr;
}

HRESULT Supervisor::disableD3dFog(Supervisor* This)
{
    if (This->d3dDisableFogFlag != 0)
    {
        AnmManager::flushSprites(g_anmManager);
        This->d3dDisableFogFlag = 0;
        return This->d3dDevice->SetRenderState(D3DRS_FOGENABLE, 0);
    }
    return 0;
}

// 0x4576b0
int Supervisor::readKeyInput()
{
    if (!g_window.isAppFocused)
        return 0;

    BYTE keyState[256]{};
    bool useDirectInput = (g_supervisor.m_gameConfig.flags & 0x400);

    if (useDirectInput)
    {
        HRESULT hr = g_supervisor.keyboard->GetDeviceState(sizeof(keyState), keyState);

        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
        {
            g_supervisor.keyboard->Acquire();
            return g_inputManager.currentState;
        }
    }
    else
        GetKeyboardState(keyState);

    int inputs = 0;

    auto isDown = [&](int dik, int vk) -> bool
    {
        int key = useDirectInput ? dik : vk;
        return (keyState[key] & 0x80) != 0;
    };

    if (isDown(DIK_Z, 'Z'))               inputs |= InputBits::SHOOT;
    if (isDown(DIK_X, 'X'))               inputs |= InputBits::BOMB;
    if (isDown(DIK_LSHIFT, VK_SHIFT))     inputs |= InputBits::FOCUS;

    // Directionals (Arrow Keys)
    if (isDown(DIK_UP, VK_UP))            inputs |= InputBits::UP;
    if (isDown(DIK_DOWN, VK_DOWN))        inputs |= InputBits::DOWN;
    if (isDown(DIK_LEFT, VK_LEFT))        inputs |= InputBits::LEFT;
    if (isDown(DIK_RIGHT, VK_RIGHT))      inputs |= InputBits::RIGHT;

    // Numpad Directionals
    if (isDown(DIK_NUMPAD8, VK_NUMPAD8))  inputs |= InputBits::UP;
    if (isDown(DIK_NUMPAD2, VK_NUMPAD2))  inputs |= InputBits::DOWN;
    if (isDown(DIK_NUMPAD4, VK_NUMPAD4))  inputs |= InputBits::LEFT;
    if (isDown(DIK_NUMPAD6, VK_NUMPAD6))  inputs |= InputBits::RIGHT;

    // High Bits
    if (isDown(DIK_ESCAPE, VK_ESCAPE))    inputs |= InputBits::ESC;
    if (isDown(DIK_LCONTROL, VK_CONTROL)) inputs |= InputBits::SKIP;
    if (isDown(DIK_Q, 'Q'))               inputs |= InputBits::Q;
    if (isDown(DIK_S, 'S'))               inputs |= InputBits::S;

    // P and Home map to the same bit (Bit 12)
    if (isDown(DIK_P, 'P') || isDown(DIK_HOME, VK_HOME)) inputs |= InputBits::HOME_P;

    if (isDown(DIK_RETURN, VK_RETURN))    inputs |= InputBits::ENTER;
    if (isDown(DIK_D, 'D'))               inputs |= InputBits::D;
    if (isDown(DIK_R, 'R'))               inputs |= InputBits::R;
    if (isDown(DIK_F10, VK_F10))          inputs |= InputBits::F10;

    if (isDown(DIK_NUMPAD7, VK_NUMPAD7))  inputs |= (InputBits::DOWN | InputBits::LEFT);  // 0x60
    if (isDown(DIK_NUMPAD9, VK_NUMPAD9))  inputs |= (InputBits::DOWN | InputBits::RIGHT); // 0xA0
    if (isDown(DIK_NUMPAD1, VK_NUMPAD1))  inputs |= (InputBits::UP | InputBits::RIGHT);   // 0x90
    if (isDown(DIK_NUMPAD3, VK_NUMPAD3))  inputs |= (InputBits::UP | InputBits::LEFT);    // 0x50

    int finalInputState = updateJoystickState(inputs);

    // Update global input history
    g_inputManager.previousState = g_inputManager.currentState;
    g_inputManager.currentState = finalInputState;
    g_inputManager.update(&g_inputManager);
    return finalInputState;
}

int Supervisor::updateJoystickState(int keyboardInput)
{
    int inputState = keyboardInput;

    // Check if we should use the Win32 Multimedia API
    if (g_supervisor.m_gameConfig.flags & 0x800)
    {
        JOYINFOEX joyInfo;
        memset(&joyInfo, 0, sizeof(joyInfo));
        joyInfo.dwSize = sizeof(joyInfo);
        joyInfo.dwFlags = JOY_RETURNALL;

        if (joyGetPosEx(JOYSTICKID1, &joyInfo) != JOYERR_NOERROR)
            return inputState;

        if (g_defaultGameConfig.shootKey >= 0 && (joyInfo.dwButtons & (1 << g_defaultGameConfig.shootKey)))
            inputState |= InputBits::SHOOT;

        if (g_defaultGameConfig.bombKey >= 0 && (joyInfo.dwButtons & (1 << g_defaultGameConfig.bombKey)))
            inputState |= InputBits::BOMB;

        if (g_defaultGameConfig.pauseKey >= 0 && (joyInfo.dwButtons & (1 << g_defaultGameConfig.pauseKey)))
            inputState |= InputBits::PAUSE;

        if (g_defaultGameConfig.focusKey >= 0 && (joyInfo.dwButtons & (1 << g_defaultGameConfig.focusKey)))
            inputState |= InputBits::FOCUS;

        if (g_defaultGameConfig.skipKey >= 0 && (joyInfo.dwButtons & (1 << g_defaultGameConfig.skipKey)))
            inputState |= InputBits::SKIP;

        int xRange = g_joyAxisX_Max - g_joyAxisX_Min;
        int xCenter = g_joyAxisX_Min + (xRange / 2);
        int xVal = (int)joyInfo.dwXpos - xCenter;
        int xThreshold = xRange / 4;

        if (xVal < -xThreshold) inputState |= InputBits::LEFT;
        if (xVal > xThreshold)  inputState |= InputBits::RIGHT;

        int yRange = g_joyAxisY_Max - g_joyAxisY_Min;
        int yCenter = g_joyAxisY_Min + (yRange / 2);
        int yVal = (int)joyInfo.dwYpos - yCenter;
        int yThreshold = yRange / 4;

        if (yVal < -yThreshold) inputState |= InputBits::UP;    // 0x10
        if (yVal > yThreshold)  inputState |= InputBits::DOWN;  // 0x20

    }
    else
    {
        HRESULT hr = g_supervisor.joystick->Poll();
        if (FAILED(hr))
        {
            hr = g_supervisor.joystick->Acquire();
            while (hr == DIERR_INPUTLOST)
            {
                hr = g_supervisor.joystick->Acquire();
                static int retryCount = 0;
                if (++retryCount >= 400)
                    break;
            }
            if (FAILED(hr))
                return inputState;
        }

        // Get State
        DIJOYSTATE2 js;
        memset(&js, 0, sizeof(js));
        hr = g_supervisor.joystick->GetDeviceState(sizeof(DIJOYSTATE2), &js);
        if (FAILED(hr))
            return inputState;

        auto isBtnDown = [&](int16_t keyIndex)
        {
            if (keyIndex < 0) 
                return false;
            return (js.rgbButtons[keyIndex] & 0x80) != 0;
        };

        if (isBtnDown(g_defaultGameConfig.shootKey)) inputState |= InputBits::SHOOT;
        if (isBtnDown(g_defaultGameConfig.bombKey))  inputState |= InputBits::BOMB;
        if (isBtnDown(g_defaultGameConfig.pauseKey)) inputState |= InputBits::PAUSE;
        if (isBtnDown(g_defaultGameConfig.focusKey)) inputState |= InputBits::FOCUS;
        if (isBtnDown(g_defaultGameConfig.skipKey)) inputState |= InputBits::SKIP;

        int deadX = g_supervisor.m_gameConfig.padDeadzoneX;
        int deadY = g_supervisor.m_gameConfig.padDeadzoneY;

        if (js.lX < -deadX) inputState |= InputBits::LEFT;
        if (js.lX > deadX)  inputState |= InputBits::RIGHT;
        if (js.lY < -deadY) inputState |= InputBits::UP;
        if (js.lY > deadY)  inputState |= InputBits::DOWN;
    }

    return inputState;
}

void Supervisor::cleanup(Supervisor* This)
{
   
}