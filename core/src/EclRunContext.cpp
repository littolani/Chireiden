#include "EclRunContext.h"

void EclRunContext::run(EclRunContext* This, float gameSpeed)
{
#if 0
    if (This->currentInstruction)
        return;

    if ((float)This->currentInstruction->time <= This->time)
    {
        do {
            EclInstruction* ci = This->currentInstruction;
            if ((This->difficultyMask & ci->difficultyMask) == 0)
                //goto switchD_0045b8b6_caseD_0;
                return;
            switch (ci->opcode)
            {
            // Nop
            case 0:
                break;

            // Stop
            case 1:
                This->currentInstruction = nullptr;

            default:
            {
                int i = This->enemy->eclVm.vfunction1();
                if (i == -1) {
                    return;
                }
                break;
            }
            }
        }
    }
    while (0);
#endif
}