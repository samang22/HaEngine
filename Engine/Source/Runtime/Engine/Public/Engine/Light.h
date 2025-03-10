#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Light.generated.h"

UCLASS(/*Abstract*/)
class ENGINE_API ALight : public AActor
{
    GENERATED_BODY()

private:
    class ULightComponent* LightComponent = nullptr;
    class UStaticMeshComponent* LightDirectionMesh = nullptr;

public:
    ALight(const FObjectInitializer& ObjectInitializer);

public:
    class ULightComponent* GetLightComponent() const { return LightComponent; }
    virtual void OnPropertyChanged(FProperty& Property);
};