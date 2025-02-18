#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

AStaticMeshActor::AStaticMeshActor()
{
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = Cast<USceneComponent>(StaticMeshComponent);
}