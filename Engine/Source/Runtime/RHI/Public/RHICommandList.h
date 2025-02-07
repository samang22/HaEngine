#pragma once

#include "CoreTypes.h"
#include "DynamicRHI.h"
#include "RHIContext.h"
#include "RHIAccess.h"

/** Get the best default resource state for the given texture creation flags */
extern RHI_API ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage, bool bInHasInitialData);

class FRHICommandList
{
public:
	RHI_API void InitializeImmediateContexts();

private:
	// 그래픽 명령이 기록되는 활성 컨텍스트.
	IRHICommandContext* GraphicsContext = nullptr;
};

class FRHICommandListExecutor
{
public:
	FRHICommandListExecutor()
	{
	}

	// GetImmediateCommandList
	static inline FRHICommandList& GetImmediateCommandList();

private:
	FRHICommandList CommandListImmediate;
};
extern RHI_API FRHICommandListExecutor GRHICommandList;

inline FRHICommandList& FRHICommandListExecutor::GetImmediateCommandList()
{
	//check(IsInRenderingThread());
	return GRHICommandList.CommandListImmediate;
}
