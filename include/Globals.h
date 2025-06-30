#pragma once
#include "Chireiden.h"
#include "Chain.h"
#include "FileAbstrction.h"
#include "Supervisor.h"
#include "AnmManager.h"

extern Supervisor g_supervisor;
extern Chain* g_chain;
extern HANDLE g_app;
extern DWORD g_unusualLaunchFlag;
extern HINSTANCE g_hInstance;
extern DWORD g_primaryScreenWorkingArea;
extern DWORD g_mouseSpeed;
extern DWORD g_screenWorkingArea;
extern LARGE_INTEGER g_performanceFrequency;
extern LARGE_INTEGER g_performanceCount;
extern PbgArchive g_pbgArchive; 
extern float g_gameSpeed;
extern AnmManager* g_anmManager;
extern D3DFORMAT g_d3dFormats[];
extern uint32_t g_bytesPerPixelLookupTable[];
extern double g_time;
extern int g_numEntriesInDatFile;
extern PbgArchive g_pbgArchives[20];