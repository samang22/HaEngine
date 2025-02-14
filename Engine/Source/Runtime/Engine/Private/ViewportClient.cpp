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

class FTestVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		Elements.push_back(FVertexElement(0, 0, VET_Float3, 0, sizeof(FVector3D)));
		VertexDeclarationRHI = GDynamicRHI->RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};
TGlobalResource<FTestVertexDeclaration> GTestVertexDeclaration;

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

	{
		TShaderMapRef<FTestVS> VertextShader;
		TShaderMapRef<FTestPS> PixelShader;

		FVertexDeclarationElementList Elements;
		Elements.push_back(FVertexElement(0, 0, VET_Float3, 0, sizeof(FVector3D)));
		FVertexDeclarationRHIRef VertexDeclarationRHI = GDynamicRHI->RHICreateVertexDeclaration(Elements);

		FRHIResourceCreateInfo CreateInfo(TEXT("MyVertexBuffer"));
		GetCommandList();

		GetCommandList().SetBoundShaderState(
			GDynamicRHI->RHICreateBoundShaderState(
				VertexDeclarationRHI,
				VertextShader.GetVertexShader(),
				PixelShader.GetPixelShader()
			).GetReference()
		);
	}

	FRHICommandListExecutor::GetImmediateCommandList().EndDrawingViewport(Viewport, true, false);
}
