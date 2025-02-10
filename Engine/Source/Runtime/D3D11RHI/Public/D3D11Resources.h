#pragma once
#include "RHIResources.h"
#include "D3D11ThirdParty.h"

/** Texture base class. */
class FD3D11Texture final : public FRHITexture //, public FD3D11ViewableResource
{
public:
    D3D11RHI_API explicit FD3D11Texture(
        const FRHITextureCreateDesc& InDesc,
        ID3D11Resource* InResource,
        ID3D11ShaderResourceView* InShaderResourceView,
        int32 InRTVArraySize,
        bool bInCreatedRTVsPerSlice,
        TArray<TRefCountPtr<ID3D11RenderTargetView>> InRenderTargetViews,
        TArray<TRefCountPtr<ID3D11DepthStencilView>> InDepthStencilViews
    );
};