#include "Components/StaticMeshComponent.h"

UStaticMeshComponent::UStaticMeshComponent()
{
}

void UStaticMeshComponent::SetStaticMesh(TEnginePtr<UStaticMesh> NewStaticMesh)
{
    if (StaticMesh == NewStaticMesh)
    {
        return;
    }

    StaticMesh = NewStaticMesh;
}