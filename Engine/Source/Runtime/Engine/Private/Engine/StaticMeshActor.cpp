#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

AStaticMeshActor::AStaticMeshActor()
{
    StaticMeshComponent;
    RootComponent = Cast<USceneComponent>(StaticMeshComponent);
}