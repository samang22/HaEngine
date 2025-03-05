#include "Engine/GameViewportClient.h"
#include "RenderResource.h"
#include "RendererInterface.h"
#include "SceneView.h"
#include "EngineModule.h"

void UGameViewportClient::Tick(float DeltaTime)
{
}

void UGameViewportClient::Draw()
{
    if (!GetScene()) { return; }
    // 뷰포트를 위한 FSceneViewFamily/FSceneView 설정
    FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene()));

    GetRendererModule().BeginRenderingViewFamily(&ViewFamily);
}