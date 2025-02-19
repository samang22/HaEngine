#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/AssetManager.h"

AStaticMeshActor::AStaticMeshActor()
{
    UStaticMesh* StaticMesh = FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::EngineConfigDir() + L"/MyCube.FBX");

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = Cast<USceneComponent>(StaticMeshComponent);
}