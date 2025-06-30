#include <stdint.h>
#include <windows.h>
#include "Chain.h"
#include "Globals.h"
#include "Supervisor.h"

void Chain::release()
{
    //Supervisor::closeThread(&g_supervisor.loadingThread);
    timeToRemove = 1;
    runCalcChain();
    releaseSingleChain(&calcChain);
    releaseSingleChain(&drawChain);
    drawChain.jobRunDrawChainCallback = nullptr;
    drawChain.registerChainCallback = nullptr;
    drawChain.runCalcChainCallback = nullptr;
    calcChain.jobRunDrawChainCallback = nullptr;
    calcChain.registerChainCallback = nullptr;
    calcChain.runCalcChainCallback = nullptr;
}

void Chain::cut(ChainElem* elementToRemove)
{
    if (!elementToRemove)
        return;

    // Search calcChain
    ChainElem* temp = calcChain.jobTrackerNode;
    while (temp)
    {
        temp = temp->nextNode;
        if (temp->jobNode == elementToRemove)
            break;
    }

    // If not found, search drawChain
    if (!temp)
    {
        temp = drawChain.jobTrackerNode;
        while (temp && temp->jobNode != elementToRemove)
            temp = temp->nextNode;

        if (!temp)
            return;
    }

    // Remove tracker from list
    if (temp->trackerPrevNode)
    {
        if (temp->nextNode)
            temp->nextNode->trackerPrevNode = temp->trackerPrevNode;

        if (temp->trackerPrevNode)
            temp->trackerPrevNode->nextNode = temp->nextNode;

        temp->nextNode = nullptr;
        temp->trackerPrevNode = nullptr;
    }

    // Cleanup elementToRemove (job)
    elementToRemove->jobRunDrawChainCallback = nullptr;
    if (((uint32_t)elementToRemove->nextNode & 1) != 0)
    {
        elementToRemove->jobRunDrawChainCallback = nullptr;
        elementToRemove->registerChainCallback = nullptr;
        elementToRemove->runCalcChainCallback = nullptr;
        free(elementToRemove);
    }
}

int Chain::runCalcChain()
{

}


int Chain::runDrawChain()
{
    
}

void Chain::releaseSingleChain(ChainElem* chainElem)
{

}

int Chain::registerCalcChain(ChainElem* chainElem, int priority)
{
	int registerCallbackResult = 0;
    if (chainElem->registerChainCallback != nullptr) {
        registerCallbackResult = (*chainElem->registerChainCallback)(chainElem->args);
        chainElem->registerChainCallback = nullptr;
    }
    if ((g_supervisor.criticalSectionFlag & 0x8000) != 0) {
        EnterCriticalSection(g_supervisor.criticalSections);
        ++g_supervisor.criticalSectionCounters[0];
    }

    chainElem->jobPriority = priority;
    ChainElem* trackerElem = g_chain->calcChain.jobNextTrackerChain;
    ChainElem* trackerChain = g_chain->calcChain.jobTrackerNode;

    while (trackerElem) 
    {
        ChainElem* temp = trackerChain->nextNode;
        if (temp->trackerPrevNode->jobPriority >= priority)
            break;
        trackerElem = temp->nextNode;
        trackerChain = temp;
    }

    if (trackerChain->nextNode)
    {
        chainElem->jobNextTrackerChain = trackerChain->nextNode;
    	trackerChain->nextNode->trackerPrevNode = chainElem->jobTrackerNode;
    }
    trackerChain->nextNode = chainElem->jobTrackerNode;
    chainElem->jobPreviousTrackerNode = trackerChain;
    if ((g_supervisor.criticalSectionFlag & 0x8000) != 0)
    {
        LeaveCriticalSection(g_supervisor.criticalSections);
        --g_supervisor.criticalSectionCounters[0];
    }
    return registerCallbackResult;
}

int Chain::registerDrawChain(ChainElem* chainElem, int priority)
{

}
