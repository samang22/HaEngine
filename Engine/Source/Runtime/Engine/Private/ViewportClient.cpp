#include "ViewportClient.h"
#include "RenderResource.h"
#include "RendererInterface.h"
#include "SceneView.h"
#include "EngineModule.h"
#include "Engine/World.h"

void UViewportClient::Init(HWND hInViewportHandle, UWorld* InWorld)
{
    hViewportHandle = hInViewportHandle;
    World = InWorld;

	{
		RECT WindowRect;
		GetClientRect(hViewportHandle, &WindowRect);
		ViewportSize.x = WindowRect.right;
		ViewportSize.y = WindowRect.bottom;
	}

	Viewport = GDynamicRHI->RHICreateViewport(hInViewportHandle, ViewportSize.x, ViewportSize.y, false, PF_A2B10G10R10);
}

void UViewportClient::Tick(float DeltaTime)
{
}

void UViewportClient::Draw()
{
	if (!GetScene()) { return; }

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene()));

	GetRendererModule().BeginRenderingViewFamily(&ViewFamily);
}

void UViewportClient::RequestResize(const uint32 NewSizeX, const uint32 NewSizeY)
{
	Viewport->Resize(NewSizeX, NewSizeY);
}

FSceneInterface* UViewportClient::GetScene() const
{
	return World->Scene;
}
