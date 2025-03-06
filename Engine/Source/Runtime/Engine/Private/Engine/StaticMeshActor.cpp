#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/AssetManager.h"
#include "RHIStaticStates.h"
#include "Materials/Material.h"

AStaticMeshActor::AStaticMeshActor()
{
	TEnginePtr<UStaticMesh> StaticMesh = FAssetManager::Get()->LoadAsset<UStaticMesh>(
		FPaths::ContentDir() + L"/Cube.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Cone.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Cylinder.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Plane.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/SK_SMG11_X.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/SK_KA47_X.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Sphere.FBX");
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -200.f));

	for (int i = 0; i < StaticMeshComponent->GetMaterialCount(); ++i)
	{
		TObjectPtr<UMaterial> NewMaterial = NewObject<UMaterial>(StaticMeshComponent, nullptr, TEXT("Material"), RF_NoFlags, StaticMesh->GetMaterial(i));
		StaticMeshComponent->SetMaterial(NewMaterial, i);
	}

	ChildStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChildStaticMeshComponent"));
	ChildStaticMeshComponent->SetStaticMesh(StaticMesh);
	ChildStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -200.f));
	ChildStaticMeshComponent->SetupAttachment(StaticMeshComponent);

	RootComponent = Cast<USceneComponent>(StaticMeshComponent);
}

void AStaticMeshActor::OnPropertyChanged(FProperty&)
{
	for (uint32 i = 0; i < StaticMeshComponent->GetMaterialCount(); ++i)
	{
		UMaterial* Material = StaticMeshComponent->GetMaterial(i);
		Material->SetRasterizerState((ERasterizerState)RasterizerState);
	}
}

void AStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

void AStaticMeshActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	FRotator Rotator = ChildStaticMeshComponent->GetRelativeRotation();
	Rotator.Yaw += 360.f * DeltaSeconds;
	Rotator.Normalize();
	ChildStaticMeshComponent->SetRelativeRotation(Rotator);
}
