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
			for (const FStaticMeshDrawCommand& StaticMeshDrawCommand : MeshDrawCommands)
			{
				for (FStaticMeshRenderData& RenderData : StaticMeshDrawCommand.RenderDatas)
				{
					if (!UniformBuffer)
					{
						const FConstantBufferInfo& ConstantBufferInfo = RenderData.Material->GetVertexShader()->GetConstantBufferInfo(TEXT("FSceneUniformBuffer"));
						UniformBuffer = RHICreateUniformBuffer(ConstantBufferInfo, &SceneUniformBuffer, sizeof(SceneUniformBuffer));
					}

					GetCommandList().SetBoundShaderState(
						GDynamicRHI->RHICreateBoundShaderState(
							RenderData.VertexFactory.GetDeclaration(),
							RenderData.Material->GetVertexShaderRHI(),
							RenderData.Material->GetPixelShaderRHI()
						).GetReference()
					);

					FMatrix Matrix = FMatrix::CreatePerspectiveFieldOfView(3.14f / 4.f, 16.f / 9.f, 0.1f, 1000.f);
					SceneUniformBuffer.ProjectionMatrix = Matrix.Transpose();
					RHIUpdateUniformBuffer(UniformBuffer, &SceneUniformBuffer, sizeof(SceneUniformBuffer));
					GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Vertex, UniformBuffer);

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
