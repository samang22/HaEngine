#include "RendererModule.h"
#include "ScenePrivate.h"
#include "SceneView.h"
#include "SceneRendering.h"
#include "StaticMeshSceneProxy.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "RHIStaticStates.h"

#include "Components/DirectionalLightComponent.h"
#include "LightSceneProxy.h"

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
	FStaticMeshDrawCommand(FPrimitiveSceneProxy* InProxy, TArray<FStaticMeshRenderData>& InRenderDatas, TArray<TObjectPtr<UMaterial>>& InOverrideMaterials)
		: Proxy(InProxy), RenderDatas(InRenderDatas), OverrideMaterials(InOverrideMaterials) 
	{
    }

    FPrimitiveSceneProxy* Proxy;
    TArray<FStaticMeshRenderData>& RenderDatas;
	TArray<TObjectPtr<UMaterial>>& OverrideMaterials;
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
		PositionData.emplace_back(-0.5f, -0.5f, 0.0f);
		PositionData.emplace_back(0.5f, -0.5f, 0.0f);
		PositionData.emplace_back(0.0f, 0.5f, 0.0f);

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
	FSceneTextures::InitializeViewFamily(ViewFamily);
	FSceneTextures& SceneTextures = GetActiveSceneTextures();

	{
		FRHITexture* RenderTargets[1] = { SceneTextures.Color.Target };
		FRHIRenderPassInfo RPInfo(1, RenderTargets, ERenderTargetActions::Clear_Store, SceneTextures.Depth.Target, EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);
		GetCommandList().BeginRenderPass(RPInfo, TEXT("BasePass"));

		FRHIDepthStencilState* DepthStencilState = TStaticDepthStencilState<true>().GetRHI();
		GetCommandList().SetDepthStencilState(DepthStencilState);

		TArray<FStaticMeshDrawCommand> MeshDrawCommands;

		FScene* Scene = (FScene*)ViewFamily.Scene;
		TShaderMapRef<FMaterialVS> MaterialVS;
		TShaderMapRef<FMaterialPS> MaterialPS; 
		
		{
			const FConstantBufferInfo& ConstantBufferInfo = MaterialPS->GetConstantBufferInfo(TEXT("FLightShaderParameters"));
			FLightShaderParameters DirectionalLightShaderParameters;

			int32 DirectionalLightCount = 0;
			for (FLightSceneProxy* Proxy : Scene->LightSceneProxies) 
			{
				TEnginePtr<UDirectionalLightComponent> DirectionalLightComponent = Cast<UDirectionalLightComponent>(Proxy->GetLightComponent());
				if (DirectionalLightComponent)
				{
					if (DirectionalLightCount > 1)
					{
						E_LOG(Warning, TEXT("UDirectionalLightComponent: {}. 1개만 지원합니다."), DirectionalLightCount);
					}

					++DirectionalLightCount;
					Proxy->GetLightShaderParameters(DirectionalLightShaderParameters);
				}
			}

			FUniformBufferRHIRef LightUniformBufferRHI = RHICreateUniformBuffer(ConstantBufferInfo, &DirectionalLightShaderParameters, sizeof(DirectionalLightShaderParameters));
			GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Pixel, LightUniformBufferRHI);
		}

		//GetCommandList().BeginRenderPass(
			//[this, MeshDrawCommands = move(MeshDrawCommands)]()
		{
			for (FPrimitiveSceneProxy* Proxy : Scene->PrimitiveSceneProxies)
			{
				UPrimitiveComponent* PrimitiveComponent = Proxy->GetPrimitiveComponent();
				PrimitiveComponent->UpdateComponentToWorld();

				if (UStaticMeshComponent* StaticMeshComponent = dynamic_cast<UStaticMeshComponent*>(PrimitiveComponent))
				{
					TEnginePtr<UStaticMesh> StaticMesh = StaticMeshComponent->GetStaticMesh();
					TArray<FStaticMeshRenderData>& RenderDatas = StaticMesh->GetRenderData();
					TArray<TObjectPtr<UMaterial>>& OverrideMaterials = StaticMeshComponent->GetOverrideMaterials();
					FMatrix RenderMatrix = StaticMeshComponent->GetRenderMatrix();
					Proxy->SetTransform(RenderMatrix);
					MeshDrawCommands.emplace_back(Proxy, RenderDatas, OverrideMaterials);
				}
			}

			const FConstantBufferInfo& ConstantBufferInfo = MaterialVS->GetConstantBufferInfo(TEXT("FSceneUniformBuffer"));
			SceneUniformBuffer.EyePosition = ViewFamily.EyePosition;
			SceneUniformBuffer.ViewMatrix = ViewFamily.ViewMatrix.Transpose();
			SceneUniformBuffer.ProjectionMatrix = ViewFamily.ProjectionMatrix.Transpose();
			SceneUniformBuffer.ViewProjectionMatrix = ViewFamily.ViewProjectionMatrix.Transpose();
			SceneUniformBufferRHI = RHICreateUniformBuffer(ConstantBufferInfo, &SceneUniformBuffer, sizeof(SceneUniformBuffer));
			GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Vertex, SceneUniformBufferRHI);
			GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Pixel, SceneUniformBufferRHI);

			for (const FStaticMeshDrawCommand& StaticMeshDrawCommand : MeshDrawCommands)
			{
				int Index = 0;
				StaticMeshDrawCommand.OverrideMaterials;
				for (FStaticMeshRenderData& RenderData : StaticMeshDrawCommand.RenderDatas)
				{
					UMaterial* Material = RenderData.Material.get();
					if (StaticMeshDrawCommand.OverrideMaterials[Index])
					{
						Material = StaticMeshDrawCommand.OverrideMaterials[Index].get();
					}

					GetCommandList().SetBoundShaderState(
						GDynamicRHI->RHICreateBoundShaderState(
							RenderData.VertexFactory.GetDeclaration(),
							Material->GetVertexShaderRHI(),
							Material->GetPixelShaderRHI()
						).GetReference()
					);

					FObjectUniformBuffer ObjectUniformBuffer;
					ObjectUniformBuffer.WorldMatrix = StaticMeshDrawCommand.Proxy->GetTransform();
					RenderData.VertexFactory.UpdateObjectUniformBuffer(GetCommandList(), ObjectUniformBuffer);
					GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);

					// [깊이 반전(근평면 1.f)] 임시로 여기서 일괄 처리
					//{
					//    // 반전된 깊이 스텐실 상태 설정
					//    D3D11_DEPTH_STENCIL_DESC invertedDepthStencilDesc;
					//    ZeroMemory(&invertedDepthStencilDesc, sizeof(invertedDepthStencilDesc));
					//    invertedDepthStencilDesc.DepthEnable = TRUE;
					//    invertedDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
					//    invertedDepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; // 반전된 깊이 비교 함수

					//    TRefCountPtr<ID3D11DepthStencilState> invertedDepthStencilState;
					//    Direct3DDevice->CreateDepthStencilState(&invertedDepthStencilDesc, invertedDepthStencilState.GetInitReference());
					//    Direct3DDeviceIMContext->OMSetDepthStencilState(invertedDepthStencilState, 1);
					//}

					GetCommandList().SetRasterizerState(Material->GetRasterizerState());
					GetCommandList().SetStreamSource(0, RenderData.VertexFactory.GetVertexBufferRHI(), 0);
					GetCommandList().DrawIndexedPrimitive(RenderData.VertexFactory.GetIndexBufferRHI(), 0, 0,
						RenderData.NumVertices, 0, RenderData.NumPrimitives, 1);

					++Index;
				}
			}
		}
		GetCommandList().EndRenderPass();
	}

	GetCommandList().CopyTexture(SceneTextures.Color.Target, ViewFamily.RenderTarget->GetRenderTarget(), FRHICopyTextureInfo());

	// UI Render
	{
		GetCommandList().BeginDrawingViewport(ViewFamily.RenderTarget, FTextureRHIRef());

		FRHIDepthStencilState* DepthStencilState = TStaticDepthStencilState<false>().GetRHI();
		GetCommandList().SetDepthStencilState(DepthStencilState);

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

			FRHIRasterizerState* RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_CW>::GetRHI();
			GetCommandList().SetRasterizerState(RHIRasterizerState);

			GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
			GetCommandList().SetStreamSource(0, GNDCTriangleVertexBuffer.VertexBufferRHI, 0);
			GetCommandList().DrawPrimitive(0, 1, 1);
		}

		GetCommandList().EndDrawingViewport(ViewFamily.RenderTarget, true, false);
	}
}

FViewFamilyInfo::FViewFamilyInfo(const FSceneViewFamily& InViewFamily)
	: FSceneViewFamily(InViewFamily)
{
	bIsViewFamilyInfo = true;
}