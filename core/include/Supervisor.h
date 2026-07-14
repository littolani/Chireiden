#pragma once
#include "Chireiden.h"
#include "AnmVm.h"
#include "Camera.h"
#include "GameConfig.h"
#include "Thread.h"

struct AnmLoaded;

#pragma pack(push, 1)
class Supervisor
{
public:
    HINSTANCE hInstance;                         // <0x0>
    IDirect3D9* d3dInterface0;                   // <0x4>
    IDirect3DDevice9* d3dDevice;                 // <0x8>
    IDirectInput8A* dInputInterface;             // <0xc>
    RECT windowDimensions;                       // <0x10>
    IDirectInputDevice8A* keyboard;              // <0x20>
    IDirectInputDevice8A* joystick;              // <0x24>
    int idk1;                                    // <0x28>
    LPDIDEVCAPS controllerCaps;                  // <0x2c>
    int idk2[10];                                // <0x30>
    HWND appWindow;                              // <0x58>
    D3DXMATRIX d3dMatrix1;                       // <0x5c>
    D3DXMATRIX d3dMatrix2;                       // <0x9c>
    D3DVIEWPORT9 d3dViewport;                    // <0xdc>
    D3DPRESENT_PARAMETERS m_d3dPresetParameters; // <0xf4>
    uint32_t idk3[18];                           // <0x12c>
    IDirect3D9* d3dInterface1;                   // <0x174>
    IDirect3D9* d3dInterface2;                   // <0x178>
    IDirect3D9* d3dInterface3;                   // <0x17c>
    uint32_t idk4[7];                            // <0x180>
    uint32_t currentDisplayModeWidth;            // <0x19c>
    uint32_t currentDisplayModeHeight;           // <0x1a0>
    uint32_t d3dPresentationIntervalFlag;        // <0x1a4>
    uint32_t d3dPresentBackBuferFormat;          // <0x1a8>
    IDirect3DSurface9* surfaceR0;                // <0x1ac>
    IDirect3DSurface9* surfaceR1;                // <0x1b0>
    IDirect3DSurface9* backBuffer;               // <0x1b4>
    uint32_t unknown;                            // <0x1b8>
    AnmVm* arcadeVm0;                            // <0x1bc>
    AnmVm* arcadeVm1;                            // <0x1c0>
    AnmVm* arcadeVm2;                            // <0x1c4>
    GameConfig m_gameConfig;                     // <0x1c8>
    Camera cam0;                                 // <0x204>
    Camera cam1;                                 // <0x31c>
    Camera stageCam;                             // <0x434>
    Camera* currentCam;                          // <0x54c>
    int camIndex;                                // <0x550>
    int gameMode;                                // <0x554>
    int gameModeToSwitchTo;                      // <0x558>
    int idk5;                                    // <0x55c>
    int idk6;                                    // <0x560>
    uint32_t frameSkipFlagProbably;              // <0x564>
    int idk7[5];                                 // <0x568>
    uint32_t m_noVerticalSyncFlag;               // <0x57c>
    int idk8;                                    // <0x580>
    int idk24;
    AnmLoaded* textAnm;                          // <0x588>
    int idk9;                                    // <0x58c>
    uint32_t flags;                              // <0x590>
    DWORD currentTime;                           // <0x594>
    int idk25;
    D3DCAPS9 d3dcaps;
    int idk26;
    uint32_t snapshotFlag;                       // <0x6d0>
    uint16_t* snapshotBuffer;                     // <0x6d4>
    int idk27;
    int idk28;
    int idk29;
    void* snapshotRelated;                       // <0x6e4>
    uint32_t* snapshotDataProbably;              // <0x6e8>
    char snapshotFilename[MAX_PATH];             // <0x6ec>
    Thread thread;                               // <0x7f0>
    uint32_t timeToSwitchGameMode;               // <0x80c>
    RTL_CRITICAL_SECTION criticalSections[12];   // <0x810>
    uint8_t criticalSectionCounters[12];         // <0x930>
    int anmManagerInterruptFlag;                 // <0x93c>
    uint32_t idk13[20];                          // <0x940>
    uint32_t d3dDisableFogFlag;                  // <0x990>
    int idk14[3];                                // <0x994>
    size_t th11DatSize;                          // <0x9a0>
    byte* th11DatBytes;                          // <0x9a4>
    short idk15;
    short idk16;
    short idk17;
    short idk18;
    short idk19;
    short idk20;
    short idk21;
    short idk22;
    double deltaTime;
    D3DCOLOR clearColor;                         // <0x9c0>
    
    void releaseDinputIface();
    int verifyGameConfig();
    void enterCriticalSection(size_t criticalSectionNumber);
    void leaveCriticalSection(size_t criticalSectionNumber);
    static void resetRenderState();
    static void renderFrameWithReset();
    static void setupViewport();
    static int initializeInputDevices(Supervisor* This);
    static int initD3d9Devices(D3DFORMAT d3dFormat);
    static void setupCameras(Supervisor* This);
    static void swapCameraTransformMatrices(Camera* cam);
    static void releaseSurfaces();
    static void releaseChains();
    static HRESULT disableD3dFog(Supervisor* This);
    static void cleanup(Supervisor* This);
    static int readKeyInput();
    static int updateJoystickState(int keyboardInput);
    static void initialize();
    //static ChainCallbackResult __fastcall onTick(void* This);
    //static ChainCallbackResult __fastcall onRegister(void* This);
};
#pragma pack(pop)
ASSERT_SIZE(Supervisor, 0x9c4);