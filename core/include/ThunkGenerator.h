#pragma once
#include <windows.h>
#include <cstdint>
#include <type_traits>
#include <vector>
#include <utility>

template<typename F>
inline void installHook(DWORD addr, F hookFunc)
{
    LPVOID funcAddr = reinterpret_cast<LPVOID>(hookFunc);
    DWORD relativeJump = (DWORD)funcAddr - addr - 5;
    DWORD oldProtect;
    VirtualProtect((LPVOID)addr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    *(BYTE*)addr = 0xE9; // JMP opcode
    *(DWORD*)(addr + 1) = relativeJump;
    VirtualProtect((LPVOID)addr, 5, oldProtect, &oldProtect);
}

/**
 * @brief Allocate a block of memory that holds the unbroken bytes taken from the shim's target function. This function assumes the bytes copied from the original function do not contain any relative offsets from jmp, call, etc
 * @param originalAddr address of the shim's target function
 * @param numBytesToReplace number of bytes to extract (must not slice instructions in-between and must be > 5)
 */
inline LPVOID createTrampoline(DWORD originalAddr, size_t numBytesToReplace)
{
    if (numBytesToReplace < 5)
    {
        puts("Byte length must be greater than 5 in order to fit jmp instruction\n");
        return nullptr;
    }
    
    // Extra 5 for the jmp back to the original function
    size_t numBytesTotal = numBytesToReplace + 5;

    LPVOID trampoline = VirtualAlloc(
        nullptr,
        numBytesTotal, 
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );

    if (!trampoline)
    {
        puts("VirtualAlloc failed\n");
        return nullptr;
    }

    memcpy(trampoline, (LPVOID)originalAddr, numBytesToReplace);

    DWORD returnTarget = originalAddr + numBytesToReplace;
    DWORD trampolineJmpAddr = (DWORD)trampoline + numBytesToReplace;

    *(BYTE*)trampolineJmpAddr = 0xe9;
    *(DWORD*)(trampolineJmpAddr + 1) = returnTarget - (trampolineJmpAddr + 5);

    DWORD oldProtect;
    VirtualProtect(trampoline, numBytesTotal, PAGE_EXECUTE_READWRITE, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), trampoline, numBytesTotal);
    return trampoline;
}

inline void installTrampoline(DWORD addr, LPVOID hookFunc, size_t numBytesToReplace)
{
    if (numBytesToReplace < 5)
    {
        puts("Byte length must be greater than 5 in order to fit jmp instruction\n");
        return;
    }

    DWORD oldProtect;
    VirtualProtect((LPVOID)addr, numBytesToReplace, PAGE_EXECUTE_READWRITE, &oldProtect);
    *(BYTE*)addr = 0xe9;
    *(DWORD*)(addr + 1) = (DWORD)hookFunc - addr - 5;

    for (int i = 5; i < numBytesToReplace; ++i)
        *(BYTE*)(addr + i) = 0x90; // NOP

    VirtualProtect((LPVOID)addr, numBytesToReplace, oldProtect, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), (LPVOID)addr, numBytesToReplace);
}

// Represents a value located on the stack at [EBP + Offset]
template <int Offset>
struct Stack { static constexpr int offset = Offset; };

enum class RegCode : uint32_t
{
    AL  = 0x0100, CL  = 0x0101, DL  = 0x0102, BL  = 0x0103,
    AH  = 0x0104, CH  = 0x0105, DH  = 0x0106, BH  = 0x0107,
    AX  = 0x0200, CX  = 0x0201, DX  = 0x0202, BX  = 0x0203,
    SP  = 0x0204, BP  = 0x0205, SI  = 0x0206, DI  = 0x0207,
    EAX = 0x0400, ECX = 0x0401, EDX = 0x0402, EBX = 0x0403,
    ESP = 0x0404, EBP = 0x0405, ESI = 0x0406, EDI = 0x0407
};

constexpr uint8_t getId(RegCode r)
{
    return static_cast<uint32_t>(r) & 0xf;
}

constexpr uint8_t getSize(RegCode r)
{
    return (static_cast<uint32_t>(r) >> 8) & 0xff;
}

// Represents a value in a CPU register (8, 16, or 32-bit)
template <RegCode R>
struct Reg
{
    static constexpr RegCode regCode = R;
};

struct Void {};

template<typename T> 
struct IsRegWrapper : std::false_type {};

template<RegCode R>
struct IsRegWrapper<Reg<R>> : std::true_type
{
    static constexpr RegCode regCode = R;
};

template <RegCode R>
struct Returns { static constexpr RegCode regCode = R; };

template<typename T>
struct IsReturnWrapper : std::false_type {};

template<RegCode R>
struct IsReturnWrapper<Returns<R>> : std::true_type
{
    static constexpr RegCode regCode = R;
};

class CodeEmitter
{
    std::vector<uint8_t> code;

public:
    CodeEmitter()
    {
        code.reserve(200);
    }

    void addByte(uint8_t b)
    {
        code.push_back(b);
    }

    void addWord(uint16_t w)
    {
        code.insert(code.end(),
        {
            static_cast<uint8_t>(w & 0xFF),
            static_cast<uint8_t>(w >> 8)
        });
    }

    void addDword(uint32_t d)
    {
        code.insert(code.end(),
        {
            static_cast<uint8_t>(d & 0xFF),
            static_cast<uint8_t>((d >> 8) & 0xFF),
            static_cast<uint8_t>((d >> 16) & 0xFF),
            static_cast<uint8_t>(d >> 24)
        });
    }

    // Emit: push [ebp + offset]
    void pushStack(int offset)
    {
        // Opcode FF 75 XX (assuming offset fits in 1 byte signed)
        addByte(0xFF);
        addByte(0x75);
        addByte((uint8_t)offset);
    }

    void pushReg(RegCode r)
    {
        uint8_t id = getId(r);
        uint8_t size = getSize(r);

        // Mapping for PUSHAD relative to EBP:
        // EAX (0) is at [EBP - 8]
        // ...
        // EDI (7) is at [EBP - 36]
        int8_t offset = -8 - ((id & 0x7) * 4);

        // Handling High Byte registers (AH, CH, DH, BH - IDs 4-7 with size 1)
        // If we want AH (ID 4), we want the EAX slot (ID 0).
        bool isHighByte = (size == 1 && id >= 4);
        if (isHighByte) offset = -8 - ((id - 4) * 4);

        if (size == 4)
        {
            // PUSH [EBP+Offset]
            addByte(0xFF); addByte(0x75); addByte((uint8_t)offset);
        }
        else
        {
            // MOV EAX, [EBP + Offset]
            addByte(0x8B); addByte(0x45); addByte((uint8_t)offset);

            // Shift if High Byte (AH/BH/etc)
            if (isHighByte) {
                // SHR EAX, 8
                addByte(0xC1); addByte(0xE8); addByte(0x08);
            }

            // Mask to size (standard C++ promotion)
            if (size == 1) {
                // AND EAX, 0xFF 
                addByte(0x25); addDword(0x000000FF);
            }
            else if (size == 2) {
                // AND EAX, 0xFFFF
                addByte(0x25); addDword(0x0000FFFF);
            }

            // PUSH EAX
            addByte(0x50);
        }
    }

    void call(LPVOID target, LPVOID currentIP)
    {
        addByte(0xE8);
        uint32_t rel = (uint32_t)target - ((uint32_t)currentIP + 5);
        addDword(rel);
    }

    LPVOID finalize()
    {
        LPVOID mem = VirtualAlloc(nullptr, code.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (mem) memcpy(mem, code.data(), code.size());

        // TELL THE CPU WE WROTE NEW CODE
        FlushInstructionCache(GetCurrentProcess(), mem, code.size());

        return mem;
    }

    size_t size() const { return code.size(); }

    void pushad() { addByte(0x60); }
    void popad() { addByte(0x61); }
    void pushfd() { addByte(0x9C); }
    void popfd() { addByte(0x9D); }

    // Overwrite a register saved by PUSHAD with the current value of EAX.
    // This allows a C++ return value (in EAX) to survive the POPAD instruction.
    void overwriteSavedReg(RegCode targetReg)
    {
        // PUSHAD Order (Top to Bottom / Low Addr to High Addr):
        // EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
        // Offsets from ESP: 0, 4, 8, 12, 16, 20, 24, 28

        uint8_t id = getId(targetReg);
        // Map RegID to PUSHAD offset. 
        // RegIDs: AX=0, CX=1, DX=2, BX=3, SP=4, BP=5, SI=6, DI=7
        // We need: 0->28, 1->24, 2->20, 3->16, 5->8, 6->4, 7->0
        // Formula: (7 - (id & 7)) * 4

        uint8_t offset = (7 - (id & 0x7)) * 4;

        // Instruction: MOV [ESP + offset], EAX
        addByte(0x89);
        addByte(0x44);
        addByte(0x24);
        addByte(offset);
    }

    // MOV Reg32, [EBP + Offset]
    void emitLoadRegFromStack(RegCode regCode, int8_t offset)
    {
        const uint8_t regIndex = getId(regCode); // value of parameter regCode cannot be used as a constant
        // Opcode 8B, ModRM(01, reg, 101/ebp), Disp8
        addByte(0x8B);
        addByte(0x45 | (regIndex << 3));
        addByte((uint8_t)offset);
    }

    // MOV [ESP + Offset], EAX
    void emitStoreEaxToStack(int32_t offset)
    {
        // If offset is large, use 32-bit displacement (89 84 24 ...)
        if (offset >= -128 && offset <= 127)
        {
            addByte(0x89);
            addByte(0x44);
            addByte(0x24);
            addByte((int8_t)offset);
        }
        else
        {
            addByte(0x89);
            addByte(0x84);
            addByte(0x24);
            addDword(offset);
        }
    }
};

template<typename First, typename... Rest>
void ProcessArgsReverse(CodeEmitter& e)
{
    if constexpr (sizeof...(Rest) > 0) // Recurse first (Right-to-Left processing)
        ProcessArgsReverse<Rest...>(e);

    using T = First;
    if constexpr (IsRegWrapper<T>::value) // Register
        e.pushReg(IsRegWrapper<T>::regCode);
    else // Only other case is Stack
        e.pushStack(T::offset + 4); // +4 to account for ebp push
}

template <typename RetLoc, typename... ArgLocs, typename F>
LPVOID createLtoThunk(const F& targetFunc, int retN)
{
    LPVOID targetPtr = (LPVOID)(+targetFunc);
    CodeEmitter e;

    // Prologue
    e.addByte(0x55);    // push ebp
    e.addWord(0xEC8B);  // mov ebp, esp
    e.addByte(0x9C);    // pushfd
    e.addByte(0x60);    // pushad

    // Process arguments (ArgLocs explicitly isolated from RetLoc)
    if constexpr (sizeof...(ArgLocs) > 0)
        ProcessArgsReverse<ArgLocs...>(e);

    size_t callOffset = e.size();
    e.addByte(0xE8); // call rel32
    e.addDword(0);   // 32-bit placeholder

    // Cleanup Hook Arguments
    constexpr int realArgCount = sizeof...(ArgLocs);
    uint8_t stackCleanup = realArgCount * 4;

    if (stackCleanup > 0)
    {
        e.addByte(0x83); e.addByte(0xC4); // add esp, X
        e.addByte(stackCleanup);
    }

    // Handle Return Value directly via RetLoc
    if constexpr (IsRegWrapper<RetLoc>::value)
    {
        constexpr RegCode r = IsRegWrapper<RetLoc>::regCode;

        // Only overwrite if it's a GP register
        if (static_cast<uint32_t>(r) & 0x0400)
            e.overwriteSavedReg(r);
    }

    // Epilogue
    e.addByte(0x61); // popad
    e.addByte(0x9D); // popfd
    e.addByte(0x5D); // pop ebp

    // ret n
    if (retN > 0)
    {
        e.addByte(0xC2);
        e.addWord((uint16_t)retN);
    }
    // ret
    else
        e.addByte(0xC3);

    uint8_t* funcMem = (uint8_t*)e.finalize();

    uint32_t currentIP = (uint32_t)funcMem + callOffset;
    uint32_t relativeOffset = (uint32_t)targetPtr - (currentIP + 5);

    *(uint32_t*)(funcMem + callOffset + 1) = relativeOffset;

    FlushInstructionCache(GetCurrentProcess(), funcMem + callOffset + 1, sizeof(uint32_t));

    return (LPVOID)funcMem;
}

// Container for the mapping layout
template <typename... Locs>
struct Storage {};

// Container for the function signature
template <typename Ret, typename... Args>
struct Signature {};

// Stack Args: Load from [EBP+Offset] into EAX, then store at [ESP+DestOffset].
// Reg Args: Load from [EBP+Offset] into DEST_REG.
// We do Stack args first to treat registers (except EAX) as preserved until the final register load phase.
// Stack Frame Calculation: [EBP] [Ret] [Arg0] [Arg1] ...
// We assume 32-bit alignment.
template <typename TupleT, typename TupleS, size_t... Is>
void processArgs(CodeEmitter& e, std::index_sequence<Is...>)
{
    ([&]() {
        using Stor = std::tuple_element_t<Is, TupleS>;
        // Check if Stor is Stack<N>
        if constexpr (requires { Stor::offset; })
        {
            constexpr int srcOffset = 8 + (Is * 4); // Arg0 is at EBP + 8.
            constexpr int destOffset = Stor::offset;

            // MOV EAX, [EBP + src]
            e.emitLoadRegFromStack(RegCode::EAX, srcOffset);
 
            // Subtract 4 from destOffset because we are PRE-CALL
            // The CALL instruction will push 4 bytes, shifting everything up by 4.
            e.emitStoreEaxToStack(destOffset - 4);
        }
    }(), ...);

    ([&]() {
        using Stor = std::tuple_element_t<Is, TupleS>;
        if constexpr (requires { Stor::regCode; })
        {
            constexpr int srcOffset = 8 + (Is * 4); // Arg0 is at EBP + 8.
            e.emitLoadRegFromStack(Stor::regCode, srcOffset); // MOV REG, [EBP + srcOffset]
        }
    }(), ...);
}

template <typename StorageMap, typename Sig>
struct ThunkGenerator;

template <typename RetLoc, typename... ArgLocs, typename RetType, typename... ArgTypes>
struct ThunkGenerator<Storage<RetLoc, ArgLocs...>, Signature<RetType, ArgTypes...>>
{
    static_assert(sizeof...(ArgLocs) == sizeof...(ArgTypes), "Mismatch between Storage locations and Function Signature arguments");
    using FunctionType = RetType(__cdecl*)(ArgTypes...);

    static FunctionType create(uintptr_t originalFuncAddress)
    {
        CodeEmitter e;

        e.addByte(0x55); // push ebp
        e.addWord(0xE589); // mov ebp, esp
        e.addByte(0x9C); // pushfd
        e.addByte(0x60); // pushad (Order: EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI)

        // Stack Alignment
        // Force ESP to be 16-byte aligned.
        // AND ESP, 0xFFFFFFF0
        e.addByte(0x83); e.addByte(0xE4); e.addByte(0xF0);

        // Allocation
        int maxStack = 0;
        auto checkStack = [&](int offset) { if (offset > maxStack) maxStack = offset; };
        ([&]() { if constexpr (requires { ArgLocs::offset; }) checkStack(ArgLocs::offset + 4); }(), ...);

        if (maxStack > 0)
        {
            int alignedStack = (maxStack + 15) & ~15; // Keep 16-byte alignment
            e.addByte(0x81); e.addByte(0xEC); e.addDword(alignedStack);
        }

        // Setup Arguments
        processArgs<std::tuple<ArgTypes...>, std::tuple<ArgLocs...>>(
            e, std::make_index_sequence<sizeof...(ArgTypes)>{}
        );

        // Emit an immediate relative call with a placeholder offset.
        // We will patch the correct offset once the thunk memory is allocated.
        size_t callOffset = e.size();
        e.addByte(0xE8); // call rel32
        e.addDword(0);   // 32-bit placeholder

        // We must overwrite the saved EAX in the PUSHAD block with the real return value.
        // PUSHAD block is at [EBP - 36] to [EBP - 4].
        // Saved EAX is at [EBP - 8].

        //FIXME: ONLY EAX SUPPORT RIGHT NOW
        if constexpr (requires { RetLoc::regCode; })
        {
            if constexpr (RetLoc::regCode == RegCode::EAX)
            {
                // MOV [EBP - 8], EAX  (89 45 F8)
                e.addByte(0x89); e.addByte(0x45); e.addByte(0xF8);
            }
        }

        // Epilogue
        // Restore ESP from EBP to locate our saved registers
        // Stack Top was EBP - 4 (Flags) - 32 (Pushad) = EBP - 36
        // LEA ESP, [EBP - 0x24]
        e.addByte(0x8D); e.addByte(0x65); e.addByte(0xDC);

        e.addByte(0x61); // popad
        e.addByte(0x9D); // popfd
        e.addByte(0x5D); // pop ebp
        e.addByte(0xC3); // ret

        // Allocate memory and flush cache initially
        uint8_t* funcMem = (uint8_t*)e.finalize();

        // Calculate the actual relative offset. 
        // Instruction pointer moves to the end of the instruction (currentIP + 5)
        uint32_t currentIP = (uint32_t)funcMem + callOffset;
        uint32_t relativeOffset = (uint32_t)originalFuncAddress - (currentIP + 5);

        // Patch the placeholder offset in the allocated memory
        *(uint32_t*)(funcMem + callOffset + 1) = relativeOffset;

        // Ensure the CPU execution pipeline sees the patched offset
        FlushInstructionCache(GetCurrentProcess(), funcMem + callOffset + 1, sizeof(uint32_t));

        return (FunctionType)funcMem;
    }
};

template <typename StorageMap, typename SigType>
auto createCustomCallingConvention(uintptr_t addr)
{
    return ThunkGenerator<StorageMap, SigType>::create(addr);
}

using AL  = Reg<RegCode::AL>;  using CL  = Reg<RegCode::CL>;  using DL  = Reg<RegCode::DL>;  using BL  = Reg<RegCode::BL>;
using AH  = Reg<RegCode::AH>;  using CH  = Reg<RegCode::CH>;  using DH  = Reg<RegCode::DH>;  using BH  = Reg<RegCode::BH>;
using AX  = Reg<RegCode::AX>;  using CX  = Reg<RegCode::CX>;  using DX  = Reg<RegCode::DX>;  using BX  = Reg<RegCode::BX>;
using SP  = Reg<RegCode::SP>;  using BP  = Reg<RegCode::BP>;  using SI  = Reg<RegCode::SI>;  using DI  = Reg<RegCode::DI>;
using EAX = Reg<RegCode::EAX>; using ECX = Reg<RegCode::ECX>; using EDX = Reg<RegCode::EDX>; using EBX = Reg<RegCode::EBX>;
using ESP = Reg<RegCode::ESP>; using EBP = Reg<RegCode::EBP>; using ESI = Reg<RegCode::ESI>; using EDI = Reg<RegCode::EDI>;
