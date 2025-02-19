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

class FTestVertexDeclaration : public FVertexDeclaration 
{
public:
	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		Elements.push_back(FVertexElement(0, 0, VET_Float3, 0, sizeof(FVector3D)));
		VertexDeclarationRHI = GDynamicRHI->RHICreateVertexDeclaration(Elements);
	}
};
TGlobalResource<FTestVertexDeclaration> GTestVertexDeclaration;

class FNDCTriangleVertexBuffer : public FVertexBuffer
{
public:
	virtual void InitRHI(FRHICommandList& RHICmdList)
	{
		TResourceArray<FVector3D> PositionData;
		PositionData.emplace_back(0.0f, 0.5f, 0.0f);
		PositionData.emplace_back(0.5f, -0.5f, 0.0f);
		PositionData.emplace_back(-0.5f, -0.5f, 0.0f);

		FRHIResourceCreateInfo CreateInfo(TEXT("MyVertexBuffer"), &PositionData);
		VertexBufferRHI = GetCommandList().CreateVertexBuffer(PositionData.GetResourceDataSize(), BUF_Static, CreateInfo);
		if (!VertexBufferRHI)
		{
			E_LOG(Warning, TEXT("VertexBufferRHI creation failed"));
			return;
		}
	}
};

TGlobalResource<FNDCTriangleVertexBuffer> GNDCTriangleVertexBuffer;

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
		GetCommandList().SetBoundShaderState(
			GDynamicRHI->RHICreateBoundShaderState(
				GTestVertexDeclaration.VertexDeclarationRHI,
				VertextShader.GetVertexShader(),
				PixelShader.GetPixelShader()
			).GetReference()
		);
		GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
		GetCommandList().SetStreamSource(0, GNDCTriangleVertexBuffer.VertexBufferRHI, 0);
		GetCommandList().DrawPrimitive(0, 1, 1);
	}

	FRHICommandListExecutor::GetImmediateCommandList().EndDrawingViewport(Viewport, true, false);
}

void UViewportClient::RequestResize(const uint32 NewSizeX, const uint32 NewSizeY)
{
	Viewport->Resize(NewSizeX, NewSizeY);
}