
/*=============================================================================
    RHIStaticStates.h: RHI static state template definition.
=============================================================================*/

#pragma once
#include "RHI.h"
#include "RenderResource.h"


/**
 * The base class of the static RHI state classes.
 */
template<typename InitializerType, typename RHIRefType, typename RHIParamRefType>
class TStaticStateRHI
{
public:
    static RHIParamRefType GetRHI()
    {
        _ASSERT(StaticResource.StateRHI);
        return StaticResource.StateRHI;
    };

private:
    /** A resource which manages the RHI resource. */
    class FStaticStateResource : public FRenderResource
    {
    public:
        RHIRefType StateRHI;
        FStaticStateResource()
        {
            //if (GIsRHIInitialized && GRHISupportsRHIThread)
            if (GDynamicRHI)
            {
                StateRHI = InitializerType::CreateRHI();
            }
            else
            {
                BeginInitResource(this);
            }
        }

        // FRenderResource interface.
        virtual void InitRHI(FRHICommandList& RHICmdList) override
        {
            //_ASSERT(!GIsRHIInitialized || !GRHISupportsRHIThread);
            StateRHI = InitializerType::CreateRHI();
        }
        virtual void ReleaseRHI() override final
        {
            //_ASSERT(!GIsRHIInitialized || !GRHISupportsRHIThread);
            StateRHI.SafeRelease();
        }
        virtual void ReleaseResource() override final
        {
            FRenderResource::ReleaseResource();
        }

        ~FStaticStateResource()
        {
            //_ASSERT(!GIsRHIInitialized || !GRHISupportsRHIThread);
            ReleaseResource();
        }
    };

    static FStaticStateResource StaticResource;
};
template<typename InitializerType, typename RHIRefType, typename RHIParamRefType>
typename TStaticStateRHI<InitializerType, RHIRefType, RHIParamRefType>::FStaticStateResource TStaticStateRHI<InitializerType, RHIRefType, RHIParamRefType>::StaticResource = TStaticStateRHI<InitializerType, RHIRefType, RHIParamRefType>::FStaticStateResource();

/**
 * 정적 RHI 래스터라이저 상태 리소스입니다.
 * TStaticRasterizerStateRHI<...>::GetStaticState()는 원하는 설정의 래스터라이저 상태를 나타내는 FRasterizerStateRHIRef를 반환합니다.
 * 렌더링 스레드에서만 사용해야 합니다.
 */
template<ERasterizerFillMode FillMode = FM_Solid, ERasterizerCullMode CullMode = CM_None, ERasterizerDepthClipMode DepthClipMode = ERasterizerDepthClipMode::DepthClip, bool bEnableMSAA = true>
class TStaticRasterizerState : public TStaticStateRHI<TStaticRasterizerState<FillMode, CullMode, DepthClipMode, bEnableMSAA>, FRasterizerStateRHIRef, FRHIRasterizerState*>
{
public:
    FORCEINLINE static FRasterizerStateRHIRef CreateRHI()
    {
        const FRasterizerStateInitializerRHI Initializer(FillMode, CullMode, 0.0f, 0.0f, DepthClipMode, bEnableMSAA);
        return RHICreateRasterizerState(Initializer);
    }
};

/**
 * 정적 RHI 스텐실 상태 리소스입니다.
 * TStaticStencilStateRHI<...>::GetStaticState()는 원하는 설정의 스텐실 상태를 나타내는 FDepthStencilStateRHIRef를 반환합니다.
 * 렌더링 스레드에서만 사용해야 합니다.
 */
template<
    bool bEnableDepthWrite = true,
    ECompareFunction DepthTest = CF_DepthNearOrEqual,
    bool bEnableFrontFaceStencil = false,
    ECompareFunction FrontFaceStencilTest = CF_Always,
    EStencilOp FrontFaceStencilFailStencilOp = SO_Keep,
    EStencilOp FrontFaceDepthFailStencilOp = SO_Keep,
    EStencilOp FrontFacePassStencilOp = SO_Keep,
    bool bEnableBackFaceStencil = false,
    ECompareFunction BackFaceStencilTest = CF_Always,
    EStencilOp BackFaceStencilFailStencilOp = SO_Keep,
    EStencilOp BackFaceDepthFailStencilOp = SO_Keep,
    EStencilOp BackFacePassStencilOp = SO_Keep,
    uint8 StencilReadMask = 0xFF,
    uint8 StencilWriteMask = 0xFF
>
class TStaticDepthStencilState : public TStaticStateRHI<
    TStaticDepthStencilState<
    bEnableDepthWrite,
    DepthTest,
    bEnableFrontFaceStencil,
    FrontFaceStencilTest,
    FrontFaceStencilFailStencilOp,
    FrontFaceDepthFailStencilOp,
    FrontFacePassStencilOp,
    bEnableBackFaceStencil,
    BackFaceStencilTest,
    BackFaceStencilFailStencilOp,
    BackFaceDepthFailStencilOp,
    BackFacePassStencilOp,
    StencilReadMask,
    StencilWriteMask
    >,
    FDepthStencilStateRHIRef,
    FRHIDepthStencilState*
>
{
public:
    static FDepthStencilStateRHIRef CreateRHI()
    {
        FDepthStencilStateInitializerRHI Initializer(
            bEnableDepthWrite,
            DepthTest,
            bEnableFrontFaceStencil,
            FrontFaceStencilTest,
            FrontFaceStencilFailStencilOp,
            FrontFaceDepthFailStencilOp,
            FrontFacePassStencilOp,
            bEnableBackFaceStencil,
            BackFaceStencilTest,
            BackFaceStencilFailStencilOp,
            BackFaceDepthFailStencilOp,
            BackFacePassStencilOp,
            StencilReadMask,
            StencilWriteMask);

        return RHICreateDepthStencilState(Initializer);
    }
};