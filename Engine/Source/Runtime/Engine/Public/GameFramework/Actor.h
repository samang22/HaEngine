#pragma once
#include "CoreMinimal.h"
#include "Actor.generated.h"

UENUM()
enum class ESpawnActorScaleMethod : uint8
{
	/** 액터의 루트 컴포넌트의 기본 크기를 무시하고 SpawnTransform 파라미터의 값으로 고정 설정합니다 */
	OverrideRootScale, //                      UMETA(DisplayName = "Override Root Component Scale"),
	/** SpawnTransform 파라미터의 값을 액터의 루트 컴포넌트의 기본 크기와 곱합니다 */
	MultiplyWithRoot, //                       UMETA(DisplayName = "Multiply Scale With Root Component Scale"),
	SelectDefaultAtRuntime, //                 UMETA(Hidden),
};

UCLASS()
class ENGINE_API AActor : public UObject
{
	GENERATED_BODY()
public:
	AActor();
};  