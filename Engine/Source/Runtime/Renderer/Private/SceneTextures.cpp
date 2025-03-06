#include "SceneTextures.h"
#include "SceneRendering.h"

RENDERER_API void FMinimalSceneTextures::InitializeViewFamily(FViewFamilyInfo& ViewFamily)
{
    const FVector2D& ViewportSize = ViewFamily.ViewportSize;
    FSceneTextures& SceneTexture = ViewFamily.SceneTextures;

    {
        FRHITextureCreateDesc Desc;
        SceneTexture.Depth = GetCommandList().CreateTexture(Desc);
    }
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