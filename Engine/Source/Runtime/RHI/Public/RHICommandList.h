#pragma once

#include "CoreTypes.h"
#include "DynamicRHI.h"
#include "RHIContext.h"
#include "RHIAccess.h"
#include "RHIResources.h"

/** 주어진 텍스처 생성 플래그에 대한 최적의 기본 리소스 상태를 가져옵니다 */
extern RHI_API ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage, bool bInHasInitialData);

/** 주어진 버퍼 생성 플래그에 대한 최적의 기본 리소스 상태를 가져옵니다 */
extern RHI_API ERHIAccess RHIGetDefaultResourceState(EBufferUsageFlags InUsage, bool bInHasInitialData);

class FRHICommandList
{
public:
	RHI_API void InitializeImmediateContexts();

	FORCEINLINE IRHICommandContext& GetContext()
	{
		_ASSERT(GraphicsContext, TEXT("There is no active graphics context on this command list. There may be a missing call to SwitchPipeline()."));
		return *GraphicsContext;
	}

	RHI_API void BeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI);
	RHI_API void EndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync);

    /*FORCEINLINE void* LockBuffer(FRHIBuffer* Buffer, uint32 Offset, uint32 SizeRHI, EResourceLockMode LockMode)
    {
        FRHICommandListScopedPipelineGuard ScopedPipeline(*this);
        return GDynamicRHI->RHILockBuffer(*this, Buffer, Offset, SizeRHI, LockMode);
    }

    FORCEINLINE void UnlockBuffer(FRHIBuffer* Buffer)
    {
        FRHICommandListScopedPipelineGuard ScopedPipeline(*this);
        GDynamicRHI->RHIUnlockBuffer(*this, Buffer);
    }*/

    FORCEINLINE FBufferRHIRef CreateBuffer(uint32 Size, EBufferUsageFlags Usage, uint32 Stride, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
    {
        FRHIBufferDesc BufferDesc = CreateInfo.bWithoutNativeResource
            ? FRHIBufferDesc::Null()
            : FRHIBufferDesc(Size, Stride, Usage);

        //FRHICommandListScopedPipelineGuard ScopedPipeline(*this);
        FBufferRHIRef Buffer = GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, CreateInfo);
        Buffer->SetTrackedAccess_Unsafe(ResourceState);
        return Buffer;
    }

    FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
    {
        return CreateBuffer(Size, Usage | EBufferUsageFlags::VertexBuffer, 0, ResourceState, CreateInfo);
    }

    FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, FRHIResourceCreateInfo& CreateInfo)
    {
        bool bHasInitialData = CreateInfo.BulkData != nullptr;
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::VertexBuffer, bHasInitialData);
        return CreateVertexBuffer(Size, Usage, ResourceState, CreateInfo);
    }

    FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
    {
        return CreateBuffer(Size, Usage | EBufferUsageFlags::IndexBuffer, 0, ResourceState, CreateInfo);
    }

    FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, EBufferUsageFlags Usage, FRHIResourceCreateInfo& CreateInfo)
    {
        bool bHasInitialData = CreateInfo.BulkData != nullptr;
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::IndexBuffer, bHasInitialData);
        return CreateIndexBuffer(Size, Usage, ResourceState, CreateInfo);
    }

	RHI_API void SetBoundShaderState(FRHIBoundShaderState* BoundShaderState);

    FORCEINLINE void SetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset)
    {
        GetContext().RHISetStreamSource(StreamIndex, VertexBuffer, Offset);
    }
    FORCEINLINE void SetPrimitiveTopology(EPrimitiveType InPrimitiveType)
    {
        GetContext().RHISetPrimitiveTopology(InPrimitiveType);
    }

    FORCEINLINE void DrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances)
    {
        GetContext().RHIDrawPrimitive(BaseVertexIndex, NumPrimitives, NumInstances);
    }
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

inline FRHICommandList& GetCommandList()
{
    return FRHICommandListExecutor::GetImmediateCommandList();
}