#include "Chain.h"
#include "Globals.h"
#include "Supervisor.h"

Chain::Chain()
{
    calcChain.trackerPrevNode = nullptr;
    calcChain.registerChainCallback = nullptr;
    calcChain.jobPriority = 0;
    calcChain.nextNode = (ChainElem*)((uintptr_t)&calcChain & ~1);
    calcChain.embeddedTracker.trackerNextNode = nullptr;
    calcChain.embeddedTracker.trackerPrevNode = nullptr;
    calcChain.embeddedTracker.trackerJobNode = &calcChain;
    drawChain.trackerPrevNode = nullptr;
    drawChain.registerChainCallback = nullptr;
    drawChain.jobPriority = 0;
    drawChain.nextNode = (ChainElem*)((uintptr_t)&drawChain & ~1);
    drawChain.embeddedTracker.trackerNextNode = nullptr;
    drawChain.embeddedTracker.trackerPrevNode = nullptr;
    drawChain.embeddedTracker.trackerJobNode = &drawChain;
    timeToRemove = 0;
}

void Chain::releaseSingleChain(Chain* This, ChainElem* root)
{
    ChainElem* elem = (root->embeddedTracker).trackerNextNode;
    while (elem != nullptr)
    {
        ChainElem* branch = elem->trackerJobNode;
        branch = branch->nextNode;
        if (branch != nullptr)
        {
            g_supervisor.enterCriticalSection(0);
            cut(This, branch);
            g_supervisor.leaveCriticalSection(0);
        }
    }
}

void Chain::cut(Chain* This, ChainElem* elementToRemove)
{
    printf("Chain cut\n");
    if (elementToRemove == nullptr)
        return;

    // Search calcChain for the tracker node pointing to elementToRemove
    ChainElem* tracker = reinterpret_cast<ChainElem*>(&This->calcChain.embeddedTracker);
    while (tracker != nullptr)
    {
        if (tracker->trackerJobNode == elementToRemove)
        {
            removeTracker(tracker, elementToRemove);
            return;
        }
        tracker = tracker->nextNode;
    }

    // If not found, search drawChain
    tracker = reinterpret_cast<ChainElem*>(&This->drawChain.embeddedTracker);
    while (tracker != nullptr)
    {
        if (tracker->trackerJobNode == elementToRemove)
        {
            removeTracker(tracker, elementToRemove);
            return;
        }
        tracker = tracker->nextNode;
    }
}

void Chain::removeTracker(ChainElem* tracker, ChainElem* elementToRemove)
{
    // Update doubly-linked list pointers to remove tracker node
    if (tracker->trackerPrevNode != nullptr)
    {
        if (tracker->nextNode != nullptr)
            tracker->nextNode->trackerPrevNode = tracker->trackerPrevNode;

        if (tracker->trackerPrevNode != nullptr)
            tracker->trackerPrevNode->nextNode = tracker->nextNode;

        tracker->nextNode = nullptr;
        tracker->trackerPrevNode = nullptr;
    }

    // Cleanup the job node (elementToRemove)
    elementToRemove->jobRunDrawChainCallback = nullptr;
    if ((reinterpret_cast<uintptr_t>(elementToRemove->nextNode) & 1) != 0)
    {
        // Heap-allocated job node: clear additional fields and free
        elementToRemove->jobRunDrawChainCallback = nullptr;
        elementToRemove->registerChainCallback = nullptr;
        elementToRemove->runCalcChainCallback = nullptr;
        game_free(elementToRemove);
    }
}

int Chain::registerCalcChain(ChainElem* chainElem, int priority)
{
    puts("Registered calc chain\n");
    Chain* This = g_chain;
    int registerCallbackResult = 0;

    // Handle registration callback
    if (chainElem->registerChainCallback != nullptr) {
        registerCallbackResult = chainElem->registerChainCallback(chainElem->args);
        chainElem->registerChainCallback = nullptr;
    }

    g_supervisor.enterCriticalSection(0);

    chainElem->jobPriority = priority;

    // Grab the embedded tracker instead of the base ChainElem
    TrackerNode* tracker = reinterpret_cast<TrackerNode*>(&This->calcChain.embeddedTracker);
    TrackerNode* trackerNext = tracker->next;

    // Traverse the tracker linked list
    while (trackerNext != nullptr)
    {
        ChainElem* jobNode = trackerNext->ownerJob;
        if (jobNode->jobPriority >= priority)
            break;

        tracker = trackerNext;
        trackerNext = trackerNext->next;
    }

    // Link the new tracker into the list
    TrackerNode* newTracker = reinterpret_cast<TrackerNode*>(&chainElem->embeddedTracker);
    newTracker->next = trackerNext;

    if (trackerNext != nullptr)
        trackerNext->prev = newTracker;

    tracker->next = newTracker;
    newTracker->prev = tracker;

    g_supervisor.leaveCriticalSection(0);
    return registerCallbackResult;
}

// 0x456c10
int Chain::registerDrawChain(ChainElem* chainElem, int priority)
{
    int registerResult = 0;

    if (chainElem->registerChainCallback)
    {
        registerResult = chainElem->registerChainCallback(chainElem->args);
        chainElem->registerChainCallback = nullptr;
    }

    g_supervisor.enterCriticalSection(0);
    chainElem->jobPriority = priority;
    TrackerNode* head = reinterpret_cast<TrackerNode*>(&g_chain->drawChain.embeddedTracker);
    TrackerNode* current = head;
    TrackerNode* nextNode = current->next;

    while (nextNode != nullptr)
    {
        ChainElem* job = nextNode->ownerJob;
        if (job->jobPriority >= priority)
            break;

        current = nextNode;
        nextNode = current->next;
    }

    TrackerNode* newTracker = reinterpret_cast<TrackerNode*>(&chainElem->embeddedTracker);
    newTracker->next = nextNode;

    if (nextNode != nullptr)
        nextNode->prev = newTracker; // Link next -> new (if next exists)

    current->next = newTracker; // Link current -> new   
    newTracker->prev = current; // Link new -> current
    g_supervisor.leaveCriticalSection(0);
    return registerResult;
}

int Chain::runCalcChain(Chain* This)
{
    g_supervisor.enterCriticalSection(0);
    ChainElem* trackerChain = This->calcChain.embeddedTracker.trackerNextNode;
    int processedCount = 0;

    while (trackerChain)
    {
        ChainElem* startingElem = trackerChain->trackerJobNode;
        ChainElem* nextTrackerChain = trackerChain->nextNode;
        trackerChain = nextTrackerChain;

        if (!startingElem)
            continue;

        // Check whether the node is a job node
        if ((reinterpret_cast<uintptr_t>(startingElem->nextNode) & 2) != 0)
        {
            bool reExecute = false;
            bool forceContinueOuter = false;

            do {
                reExecute = false;

                if (This->timeToRemove == 0)
                {
                    g_supervisor.leaveCriticalSection(0);
                    ChainCallbackResult result = startingElem->jobRunDrawChainCallback(startingElem->args);
                    g_supervisor.enterCriticalSection(0);

                    switch (result)
                    {
                    case ContinueAndRemoveJob:
                        cut(This, startingElem);
                        processedCount++;
                        forceContinueOuter = true;
                        break;

                    case ExecuteAgain:
                        if ((reinterpret_cast<uintptr_t>(startingElem->nextNode) & 2) != 0)
                            reExecute = true;
                        break;

                    case Break:
                        g_supervisor.leaveCriticalSection(0);
                        return 1;

                    case QuitGame:
                    case ExitGameSuccess:
                        g_supervisor.leaveCriticalSection(0);
                        return 0;

                    case ExitGameError:
                        g_supervisor.leaveCriticalSection(0);
                        return -1;

                    case RestartFromFirstJob:
                        trackerChain = This->calcChain.embeddedTracker.trackerNextNode;
                        processedCount = 0;
                        forceContinueOuter = true;
                        break;

                    case CleanupAndContinue:
                        if (startingElem->runCalcChainCallback) {
                            startingElem->runCalcChainCallback(startingElem->args);
                        }
                        [[fallthrough]];

                    default:
                        break;
                    }
                }
                else
                {
                    // Destruction Path
                    if (startingElem->runCalcChainCallback)
                        startingElem->runCalcChainCallback(startingElem->args);
                }

            } while (reExecute);

            if (forceContinueOuter)
                continue;
        }
        processedCount++;
    }
    g_supervisor.leaveCriticalSection(0);
    return processedCount;
}

int Chain::runDrawChain(void)
{
    int processedCount = 0;
    Chain* pChain = g_chain;

    g_supervisor.enterCriticalSection(0);
    ChainElem* element = pChain->drawChain.embeddedTracker.trackerNextNode;

    while (element != nullptr)
    {
        // Cache the current job and the next element in the list
        ChainElem* startingElem = element->trackerJobNode;
        ChainElem* nextElement = element->nextNode;

        if (startingElem->trackerPrevNode != nullptr && (reinterpret_cast<uintptr_t>(startingElem->nextNode) & 2))
        {
            bool executeAgain;
            do {
                executeAgain = false;

                g_supervisor.leaveCriticalSection(0);
                ChainCallbackResult result = startingElem->jobRunDrawChainCallback(startingElem->args);
                g_supervisor.enterCriticalSection(0);

                switch (result)
                {
                case ContinueAndRemoveJob:
                    cut(pChain, startingElem);
                    break;

                case ExecuteAgain:
                    if (reinterpret_cast<uintptr_t>(startingElem->nextNode) & 2)
                        executeAgain = true;
                    break;

                case Break:
                    g_supervisor.leaveCriticalSection(0);
                    return 1;

                case ExitGameSuccess:
                    g_supervisor.leaveCriticalSection(0);
                    return 0;

                case ExitGameError:
                    g_supervisor.leaveCriticalSection(0);
                    return -1;

                default:
                    break;
                }
            } while (executeAgain);
        }

        processedCount++;
        element = nextElement;
    }

    g_supervisor.leaveCriticalSection(0);
    return processedCount;
}