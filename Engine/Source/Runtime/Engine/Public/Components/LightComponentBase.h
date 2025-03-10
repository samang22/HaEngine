#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LightComponentBase.generated.h"

UCLASS(/*Abstract*/)
class ENGINE_API ULightComponentBase : public USceneComponent
{
    GENERATED_BODY()

    /**
     * 빛의 색을 필터링합니다.
     * 이로 인해 빛의 유효 강도가 변경될 수 있습니다.
     */
    UPROPERTY(EditAnywhere)
    FVector LightColor;
};