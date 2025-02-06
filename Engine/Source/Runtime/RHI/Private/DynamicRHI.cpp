#include "RHI.h"
#include "CoreMinimal.h"

// Globals.
FDynamicRHI* GDynamicRHI = NULL;

void RHIInit()
{
    if (!GDynamicRHI)
    {
        GDynamicRHI = PlatformCreateDynamicRHI();

        GDynamicRHI->Init();
    }
    _ASSERT(GDynamicRHI);
}