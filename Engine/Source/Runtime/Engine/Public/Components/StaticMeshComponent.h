#pragma once
#include "Components/MeshComponent.h"
#include "StaticMeshComponent.generated.h"


UCLASS()
class ENGINE_API UStaticMeshComponent : public UMeshComponent
{
	GENERATED_BODY()
public:
	UStaticMeshComponent();
};