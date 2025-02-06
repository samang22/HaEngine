#include "RHICommandList.h"

FRHICommandListExecutor GRHICommandList;

RHI_API void FRHICommandList::InitializeImmediateContexts()
{
	GraphicsContext = ::RHIGetDefaultContext();
}
