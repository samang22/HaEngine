#include "D3D11Resources.h"

FD3D11Texture::FD3D11Texture(const FRHITextureCreateDesc& InDesc, ID3D11Resource* InResource, ID3D11ShaderResourceView* InShaderResourceView, int32 InRTVArraySize, bool bInCreatedRTVsPerSlice, TArray<TRefCountPtr<ID3D11RenderTargetView>> InRenderTargetViews, TArray<TRefCountPtr<ID3D11DepthStencilView>> InDepthStencilViews)
    : FRHITexture(InDesc)
    /*, Resource(InResource)
    , ShaderResourceView(InShaderResourceView)
    , RenderTargetViews(InRenderTargetViews)
    , RTVArraySize(InRTVArraySize)
    , bCreatedRTVsPerSlice(bInCreatedRTVsPerSlice)
    , bAlias(false)*/
{
}