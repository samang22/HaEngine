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

FUniformBufferRHIRef RHICreateUniformBuffer(const FConstantBufferInfo& Layout, const void* Contents, const uint32 ContentsSize)
{
    return GDynamicRHI->RHICreateUniformBuffer(Layout, Contents, ContentsSize);
}

FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer)
{
    //LLM_SCOPE_BYNAME(TEXT("RHIMisc/CreateRasterizerState"));
    return GDynamicRHI->RHICreateRasterizerState(Initializer);
}

FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer)
{
    return GDynamicRHI->RHICreateDepthStencilState(Initializer);
}