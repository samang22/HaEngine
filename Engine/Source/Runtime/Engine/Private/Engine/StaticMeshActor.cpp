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