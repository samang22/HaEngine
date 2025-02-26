#include "RendererModule.h"
#include "ScenePrivate.h"
#include "SceneView.h"
#include "SceneRendering.h"
#include "StaticMeshSceneProxy.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

FRendererModule::FRendererModule()
{
}

void FRendererModule::StartupModule()
{
    IRendererModule::StartupModule();
}

void FRendererModule::ShutdownModule()
{
    IRendererModule::ShutdownModule();
}

FSceneInterface* FRendererModule::AllocateScene(UWorld* World, ERHIFeatureLevel::Type InFeatureLevel)
{
    FScene* NewScene = new FScene(World, InFeatureLevel);
    AllocatedScenes.insert(NewScene);
    return NewScene;
}

void FRendererModule::RemoveScene(FSceneInterface* Scene)
{
    AllocatedScenes.erase(Scene);
    delete Scene;
}

void FRendererModule::BeginRenderingViewFamily(FSceneViewFamily* ViewFamily)
{
    FSceneRenderer* SceneRenderer = new FSceneRenderer(ViewFamily);

    {
        SceneRenderer->Render();
    }
    delete SceneRenderer;
}

FSceneRenderer::FSceneRenderer(const FSceneViewFamily* InViewFamily)
    : ViewFamily(*InViewFamily)
{
}

FSceneRenderer::~FSceneRenderer()
{
}

struct FStaticMeshDrawCommand
{
    FStaticMeshDrawCommand(FPrimitiveSceneProxy* InProxy, vector<FStaticMeshRenderData>& InRenderDatas)
        : Proxy(InProxy), RenderDatas(InRenderDatas) {
    }

    FPrimitiveSceneProxy* Proxy;
    TArray<FStaticMeshRenderData>& RenderDatas;
};


#include "Shader.h"
class FTestVS : public FShader
{
	DECLARE_SHADER_TYPE(FTestVS)
};
IMPLEMENT_SHADER_TYPE(FTestVS, FPaths::ShaderDir() + L"/VertexShader.hlsl", "VS", SF_Vertex)

class FTestPS : public FShader
{
	DECLARE_SHADER_TYPE(FTestPS)
};
IMPLEMENT_SHADER_TYPE(FTestPS, FPaths::ShaderDir() + L"/PixelShader.hlsl", "PS", SF_Pixel)

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


void FSceneRenderer::Render()
{
	FRHICommandListExecutor::GetImmediateCommandList().BeginDrawingViewport(ViewFamily.RenderTarget, FTextureRHIRef());

	/*{
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
	}*/

	TArray<FStaticMeshDrawCommand> MeshDrawCommands;

	FScene* Scene = (FScene*)ViewFamily.Scene;
	for (FPrimitiveSceneProxy* Proxy : Scene->Proxies)
	{
		UPrimitiveComponent* PrimitiveComponent = Proxy->GetPrimitiveComponent();
		PrimitiveComponent->UpdateComponentToWorld();

		if (UStaticMeshComponent* StaticMeshComponent = dynamic_cast<UStaticMeshComponent*>(PrimitiveComponent))
		{
			TEnginePtr<UStaticMesh> StaticMesh = StaticMeshComponent->GetStaticMesh();
			TArray<FStaticMeshRenderData>& RenderDatas = StaticMesh->GetRenderData();
			FMatrix RenderMatrix = StaticMeshComponent->GetRenderMatrix();
			Proxy->SetTransform(RenderMatrix);
			MeshDrawCommands.emplace_back(Proxy, RenderDatas);
		}
	}

	FRHICommandListExecutor::GetImmediateCommandList().BeginRenderPass(
		[this, MeshDrawCommands = move(MeshDrawCommands)]()
		{
			TShaderMapRef<FMaterialVS> MaterialVS;

			const FConstantBufferInfo& ConstantBufferInfo = MaterialVS->GetConstantBufferInfo(TEXT("FSceneUniformBuffer"));
			SceneUniformBufferRHI = RHICreateUniformBuffer(ConstantBufferInfo, &SceneUniformBuffer, sizeof(SceneUniformBuffer));

			SceneUniformBuffer.ViewMatrix = ViewFamily.ViewMatrix.Transpose();
			SceneUniformBuffer.ProjectionMatrix = ViewFamily.ProjectionMatrix.Transpose();
			RHIUpdateUniformBuffer(SceneUniformBufferRHI, &SceneUniformBuffer, sizeof(SceneUniformBuffer));
			GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Vertex, SceneUniformBufferRHI);


			for (const FStaticMeshDrawCommand& StaticMeshDrawCommand : MeshDrawCommands)
			{
				for (FStaticMeshRenderData& RenderData : StaticMeshDrawCommand.RenderDatas)
				{
					GetCommandList().SetBoundShaderState(
						GDynamicRHI->RHICreateBoundShaderState(
							RenderData.VertexFactory.GetDeclaration(),
							RenderData.Material->GetVertexShaderRHI(),
							RenderData.Material->GetPixelShaderRHI()
						).GetReference()
					);

					FObjectUniformBuffer ObjectUniformBuffer;
					ObjectUniformBuffer.Matrix = StaticMeshDrawCommand.Proxy->GetTransform();
					RenderData.VertexFactory.UpdateObjectUniformBuffer(GetCommandList(), ObjectUniformBuffer);
					GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
					GetCommandList().SetStreamSource(0, RenderData.VertexFactory.GetVertexBufferRHI(), 0);
					GetCommandList().DrawIndexedPrimitive(RenderData.VertexFactory.GetIndexBufferRHI(), 0, 0,
						RenderData.NumVertices, 0, RenderData.NumPrimitives, 1);
				}
			}
		}
	);

	FRHICommandListExecutor::GetImmediateCommandList().ExecuteRenderPass();

	FRHICommandListExecutor::GetImmediateCommandList().EndDrawingViewport(ViewFamily.RenderTarget, true, false);
}
