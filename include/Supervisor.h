#pragma once
#include "Chireiden.h"

struct Supervisor
{
    HINSTANCE hInstance;            // <0x0>
    IDirect3D9* d3dInterface0;      // <0x4>
    IDirect3DDevice9* d3dDevice;    // <0x8>
    IDirectInput8A* dInputInterface; // <0xc>
    RECT windowDimensions;          // <0x10>
    IDirectInputDevice8A* keyboard;  // <0x20>
    IDirectInputDevice8A* joystick;  // <0x24>
    // Unknown 0x4
    uint32_t controllerCaps;  // <0x2c>
    HWND appWindow;           // <0x58>
    D3DMATRIX d3dMatrix1;     // <0x5c>
    D3DMATRIX d3dMatrix2;     // <0x9c>
    D3DVIEWPORT9 d3dViewport; // <0xdc>
    D3DPRESENT_PARAMETERS d3dPresetParameters; // <0xf4>
    // Unknown Stuff
    IDirect3D9* d3dInterface1; // <0x174>
    IDirect3D9* d3dInterface2; // <0x178>
    IDirect3D9* d3dInterface3; // <0x17c>

    uint32_t criticalSectionFlag;
    RTL_CRITICAL_SECTION criticalSections[12]; // <0x810>
    uint8_t criticalSectionCounters[12]; // <0x930>
    size_t th11DatSize; // <0x9a0>
    byte* th11DatBytes; // <0x9a4>

    int initializeDevices();
    void releaseDinputIface();
    void enterCriticalSection(size_t criticalSectionNumber);
    void leaveCriticalSection(size_t criticalSectionNumber);

};
//ASSERT_SIZE(Supervisor, 0x9c0);
extern Supervisor g_supervisor;