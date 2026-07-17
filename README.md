# 東方地霊殿 ～ Subterranean Animism

This project aims to accurately reconstruct the source code of [Touhou Chireiden ~ Subterranean Animism 1.00a](https://en.touhouwiki.net/wiki/Subterranean_Animism) by Team Shanghai Alice.

## Decompilation Strategy

The current reconstruction methodology relies on hooking the game with equivalent reimplemented functions, testing, and iterating until the full behavior is recovered.

The project is built as a generic proxy DLL named `d3d9.dll`. When placed next to the original `th11.exe`, the game loads our DLL first, applying all hooks during initialization. We then lazy-load the actual `d3d9.dll` from the system directory when the game calls `Direct3DCreate9`.

## Getting Started

The project uses CMake and can be opened directly in Visual Studio, which will automatically generate the solution.

**Prerequisites:**
1. [Download the DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
2. Obtain the game files (purchase a copy or source an archive).
3. Place the game folder inside the project root and rename it to `game_files`.
4. Build the solution. CMake/MSVC will produce `d3d9.dll` and copy `game_files` into the output directory.

## Debugging in Visual Studio
Visual Studio should find the `launch.vs.json` in the root folder. In the `Select Startup Item` dropdown menu, choose `th11.exe`.

## Tackling Link-Time Optimization (LTO)

This executable was built with Link-Time Optimization (LTO). While beneficial for performance, LTO complicates reverse engineering by:

* Inlining functions across translation units.
* Reordering code and destroying clear call-site boundaries.
* Transforming standard calling conventions into custom register-based conventions (e.g., passing arguments in `EAX`, `EBX`, etc., rather than the stack).

This means decompiled signatures often do not match standard C/C++ prototypes. To solve this, we use a custom Thunk Generator to bridge the gap between our standard C++ implementations and the game's LTO-optimized assembly.

## Thunk Helpers (`core/include/ThunkGenerator.h`)

We provide compile-time template helpers to generate machine code thunks on the fly. These handle the translation of calling conventions.

### 1. Replacing Game Functions (`createLtoThunk`)

Use this when you have written a C++ reimplementation and want to hook the original game function. This thunk adapts the game's non-standard caller layout (registers/stack) into a standard `__cdecl` call to your function.

```cpp
// Helper: createLtoThunk<Storage...>(void* targetFunction, int retPopSize)
// Usage: Hooking AnmManager::preloadAnmFromMemory
installHook(0x454190, createLtoThunk<
    EAX,                    // Map return value to EAX
    Stack<0x4>,             // 1st arg is at ESP+0x4
    Stack<0x8>,             // 2nd arg is at ESP+0x8
    ECX                     // 3rd arg is passed in ECX
>(AnmManager::preloadAnmFromMemory, 0x8)); // Caller expects Ret 0x8 stack cleanup

// For functions returning `void`, replace the first template argument with `Void`:
installHook(0x44fd10, createLtoThunk<Void, ESI>(AnmManager::flushSprites, 0));
```

### 2. Calling Game Functions (`createCustomCallingConvention`)

Use this when you need to call a specific address in the game (e.g., drawing a specific VM) that uses a custom LTO convention. This creates a function pointer you can invoke from C++. For `void`, simply do as above.

```cpp
// Define the C++ signature
using ltoFunc = Signature<int, AnmManager*, AnmVm*>;

// Define the Assembly storage map
using ltoSig = Storage<
    EAX,                     // Return      -> EAX
    Stack<0x4>,              // AnmManager* -> Stack[0x4]
    EBX                      // AnmVm* -> EBX
>;

// Generate the callable wrapper
static auto game_drawVmWithTextureTransform = createCustomCallingConvention<ltoSig, ltoFunc>(0x4513a0);

// Call it
game_drawVmWithTextureTransform(This, vm);
```
## Hooking Global Variables

Global variables are mapped in `globals.csv`. After declaring any new global variables, update intellisense by running `python ./scripts/GenerateHeaders.py`.

## VM Architecture & Debugging

Touhou 11 relies heavily on bytecode interpreters (Virtual Machines) for different game subsystems. The primary implementations are:

* **ANM:** Sprite animations and visual effects.
* **ECL:** Enemy logic and bullet patterns.
* **MSG:** Character dialogue and cutscenes.
* **STD:** Background stage effects.

To facilitate reverse engineering these scripts, we are integrating an **ImGui/D3D9 Overlay**. This acts as a visual step-debugger for the game's internal VMs.

**Features:**

* **VM List Navigation:** Inspect active VMs by traversing the linked lists in `AnmManager` (e.g., `AnmVmList` nodes).
* **Register View:** View the internal state of a VM in real-time.
* **Int Registers:** `AnmVm::m_intVars` (General purpose integer storage).
* **Float Registers:** `AnmVm::m_floatVars` (General purpose float storage).


* **Stepping:** The goal is to allow "pausing" a specific VM and stepping through opcodes one tick at a time, similar to stepping through threads in Visual Studio.
