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
void RHIExit()
{
    if (NULL != GDynamicRHI)
    {
        // Flush any potential commands queued before we shut things down.
        //FRHICommandListExecutor::GetImmediateCommandList().ImmediateFlush(EImmediateFlushType::FlushRHIThread);

        // Destruct the dynamic RHI.
        GDynamicRHI->Shutdown();
        delete GDynamicRHI;
        GDynamicRHI = NULL;
    }
}
