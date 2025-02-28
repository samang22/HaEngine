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

	ChildStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChildStaticMeshComponent"));
	ChildStaticMeshComponent->SetStaticMesh(StaticMesh);
	ChildStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -200.f));
	ChildStaticMeshComponent->SetupAttachment(StaticMeshComponent);

	RootComponent = Cast<USceneComponent>(StaticMeshComponent);
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
