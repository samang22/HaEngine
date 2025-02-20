#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/AssetManager.h"

AStaticMeshActor::AStaticMeshActor()
{
    TEnginePtr<UStaticMesh> StaticMesh = FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::EngineConfigDir() + L"/MyCube.FBX");
    TEnginePtr<UStaticMesh> StaticMesh2 = FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::EngineConfigDir() + L"/SK_SMG11_X.FBX");
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    StaticMeshComponent->SetStaticMesh(StaticMesh2);
    RootComponent = Cast<USceneComponent>(StaticMeshComponent);
}

void BeginRenderPass(function<void()> InCmd);

void AStaticMeshActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    BeginRenderPass([this]()
        {
            TEnginePtr<UStaticMesh> StaticMesh = StaticMeshComponent->GetStaticMesh();
            TArray<FStaticMeshRenderData>& RenderDatas = StaticMesh->GetRenderData();
            for (FStaticMeshRenderData& RenderData : RenderDatas)
            {
                GetCommandList().SetBoundShaderState(
                    GDynamicRHI->RHICreateBoundShaderState(
                        RenderData.VertexFactory.Declaration,
                        RenderData.Material->GetVertexShaderRHI(),
                        RenderData.Material->GetPixelShaderRHI()
                    ).GetReference()
                );
                GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
                GetCommandList().SetStreamSource(0, RenderData.VertexFactory.VertexBuffer->VertexBufferRHI, 0);
                GetCommandList().DrawIndexedPrimitive(RenderData.VertexFactory.IndexBuffer->IndexBufferRHI, 0, 0,
                    RenderData.NumVertices, 0, RenderData.NumPrimitives, 1);
            }
        });
}
