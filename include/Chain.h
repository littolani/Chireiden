#pragma once

#include "Chireiden.h"
#include "Globals.h"

enum ChainCallbackResult
{
    CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB = 0,
    CHAIN_CALLBACK_RESULT_CONTINUE,
    CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN,
    CHAIN_CALLBACK_RESULT_BREAK,
    CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS,
    CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR,
    CHAIN_CALLBACK_RESULT_RESTART_FROM_FIRST_JOB,
    UNKNOWN_7,
    UNKNOWN_8
};

typedef ChainCallbackResult(*ChainCallback)(void*);

// ChainElem structure definition
struct ChainElem {
    union {
        ChainElem* jobNode;  // <0x0> Job: priority; 
        int jobPriority;     // <0x0> Tracker: Job node
    };

    // Bitflags: bit 0: 1? Heap-allocated : Stack-allocated
    // Bitflags: bit 1: 1? Job Node : Tracker Node
    ChainElem* nextNode;  // <0x4> Job/Tracker: next node

    union {
        ChainCallback jobRunDrawChainCallback;  // <0x8> Job: callback
        ChainElem* trackerPrevNode;             // <0x8> Tracker: prev
    };

    ChainCallback registerChainCallback;  // <0xc> Job: callback
    ChainCallback runCalcChainCallback;   // <0x10> Job: callback
    ChainElem* jobTrackerNode;            // <0x14> Job: Current tracker node
    ChainElem* jobNextTrackerChain;       // <0x18> Job: Next Tracker node
    ChainElem* jobPreviousTrackerNode;    // <0x1c> Job: Previous Tracker node
    void* args;                           // <0x20> Job: callback args
};
ASSERT_SIZE(ChainElem, 0x24);

struct Chain {
    ChainElem calcChain;                // Sentinel node for calculation chain
    ChainElem drawChain;                // Sentinel node for drawing chain
    uint32_t timeToRemove;              // Time to remove flag

    Chain() = default;
    void release();
    int runCalcChain();
    int runDrawChain();
    int registerCalcChain(ChainElem* chainElem, int priority);
    int registerDrawChain(ChainElem* chainElem, int priority);
    void releaseSingleChain(ChainElem* chainElem);
    void cut(ChainElem* elementToRemove);
};
ASSERT_SIZE(Chain, 0x4c);