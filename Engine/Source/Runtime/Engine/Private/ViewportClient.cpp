#include "ViewportClient.h"
#include "RenderResource.h"

#include "Shader.h"
class FTestVS : public FShader
{
	DECLARE_SHADER_TYPE(FTestVS)
};
IMPLEMENT_SHADER_TYPE(FTestVS, FPaths::EngineConfigDir() + L"/VertexShader.hlsl", "VS", SF_Vertex)

class FTestPS : public FShader
{
	DECLARE_SHADER_TYPE(FTestPS)
};
IMPLEMENT_SHADER_TYPE(FTestPS, FPaths::EngineConfigDir() + L"/PixelShader.hlsl", "PS", SF_Pixel)

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

void UViewportClient::Draw()
{
	FRHICommandListExecutor::GetImmediateCommandList().BeginDrawingViewport(Viewport, FTextureRHIRef());
	FRHICommandListExecutor::GetImmediateCommandList().EndDrawingViewport(Viewport, true, false);
}
