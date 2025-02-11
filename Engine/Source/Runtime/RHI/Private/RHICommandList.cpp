#include "RHICommandList.h"

FRHICommandListExecutor GRHICommandList;

RHI_API void FRHICommandList::InitializeImmediateContexts()
{
	GraphicsContext = ::RHIGetDefaultContext();
}

RHI_API void FRHICommandList::BeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI)
{
	GetContext().RHIBeginDrawingViewport(Viewport, RenderTargetRHI);
}

RHI_API void FRHICommandList::EndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync)
{
	GetContext().RHIEndDrawingViewport(Viewport, bPresent, bLockToVsync);
 }
