#include "SceneTextures.h"
#include "SceneRendering.h"

RENDERER_API void FMinimalSceneTextures::InitializeViewFamily(FViewFamilyInfo& ViewFamily)
{
    const FVector2D& ViewportSize = ViewFamily.ViewportSize;
    FSceneTextures& SceneTexture = ViewFamily.SceneTextures;

    {
        FRHITextureCreateDesc Desc =
            FRHITextureCreateDesc::Create2D(TEXT("SceneDepthZ"),
                ViewportSize.x, ViewportSize.y, PF_DepthStencil)
            .SetFlags(ETextureCreateFlags::DepthStencilTargetable | ETextureCreateFlags::ShaderResource | ETextureCreateFlags::InputAttachmentRead)
            .SetClearValue(FClearValueBinding(0.f, 0)) // Dx 기본은 근평면 0.f / 원평면 1.f이라 1로 초기화 하는것이 일반적이지만, UE에서는 반대
            .DetermineInititialState();
        SceneTexture.Depth = GetCommandList().CreateTexture(Desc);
    }

    {
        FRHITextureCreateDesc Desc =
            FRHITextureCreateDesc::Create2D(TEXT("SceneColor"),
                ViewportSize.x, ViewportSize.y, PF_A2B10G10R10) // PF_FloatRGBA
            .SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource /*| ETextureCreateFlags::UAV*/)
            .SetClearValue(FClearValueBinding(FLinearColor(0.f, 0.f, 0.f, 1.f)))
            .DetermineInititialState();
        SceneTexture.Color = GetCommandList().CreateTexture(Desc);
    }

    ViewFamily.bIsSceneTexturesInitialized = true;
}

RENDERER_API void FSceneTextures::InitializeViewFamily(FViewFamilyInfo& ViewFamily)
{
    FMinimalSceneTextures::InitializeViewFamily(ViewFamily);
}

RENDERER_API EPixelFormat FSceneTextures::GetGBufferFFormatAndCreateFlags(ETextureCreateFlags& OutCreateFlags)
{
    _ASSERT(false);
    return EPixelFormat();
}