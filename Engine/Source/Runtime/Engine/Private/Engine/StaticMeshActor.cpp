#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/AssetManager.h"

AStaticMeshActor::AStaticMeshActor()
{
	TEnginePtr<UStaticMesh> StaticMesh = FAssetManager::Get()->LoadAsset<UStaticMesh>(
		FPaths::ContentDir() + L"/Cube.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Cone.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Cylinder.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Plane.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/SK_SMG11_X.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/SM_KA47.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Sphere.FBX");
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -200.f));
	RootComponent = Cast<USceneComponent>(StaticMeshComponent);
}

void BeginRenderPass(function<void()> InCmd);

void AStaticMeshActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	BeginRenderPass([this]()
		{
			StaticMeshComponent->UpdateComponentToWorld();

			TEnginePtr<UStaticMesh> StaticMesh = StaticMeshComponent->GetStaticMesh();
			TArray<FStaticMeshRenderData>& RenderDatas = StaticMesh->GetRenderData();
			for (FStaticMeshRenderData& RenderData : RenderDatas)
			{
				GetCommandList().SetBoundShaderState(
					GDynamicRHI->RHICreateBoundShaderState(
						RenderData.VertexFactory.GetDeclaration(),
						RenderData.Material->GetVertexShaderRHI(),
						RenderData.Material->GetPixelShaderRHI()
					).GetReference()
				);

				if (!UniformBuffer)
				{
					const FConstantBufferInfo& ConstantBufferInfo = RenderData.Material->GetVertexShader()->GetConstantBufferInfo(TEXT("FSceneUniformBuffer"));
					UniformBuffer = RHICreateUniformBuffer(ConstantBufferInfo, &SceneUniformBuffer, sizeof(SceneUniformBuffer));
				}

				FMatrix Matrix = FMatrix::CreatePerspectiveFieldOfView(3.14f / 4.f, 16.f / 9.f, 0.1f, 1000.f);
				SceneUniformBuffer.ProjectionMatrix = Matrix.Transpose();

				RHIUpdateUniformBuffer(UniformBuffer, &SceneUniformBuffer, sizeof(SceneUniformBuffer));
				GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Vertex, UniformBuffer);


				FObjectUniformBuffer ObjectUniformBuffer;
				ObjectUniformBuffer.Matrix = StaticMeshComponent->GetComponentTransform().GetMatrix();

				RenderData.VertexFactory.UpdateObjectUniformBuffer(GetCommandList(), ObjectUniformBuffer);
				GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
				GetCommandList().SetStreamSource(0, RenderData.VertexFactory.GetVertexBufferRHI(), 0);
				GetCommandList().DrawIndexedPrimitive(RenderData.VertexFactory.GetIndexBufferRHI(), 0, 0,
					RenderData.NumVertices, 0, RenderData.NumPrimitives, 1);
			}
		});
}
