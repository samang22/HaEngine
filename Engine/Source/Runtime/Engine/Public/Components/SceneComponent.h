#pragma once
#include "Components/ActorComponent.h"
#include "SceneComponent.generated.h"

/**
 * SceneComponent는 Transform을 가지며 Attach를 지원하지만, 렌더링이나 충돌 기능은 없습니다.
 * Useful as a 'dummy' component in the hierarchy to offset others.
 * @see [Scene Components](https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html#scenecomponents)
 */
UCLASS()
class ENGINE_API USceneComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USceneComponent();

	/** Get the SceneComponent we are attached to. */
	//UFUNCTION(BlueprintCallable, Category = "Transformation")
	USceneComponent* GetAttachParent() const;

	/** 현재 컴포넌트의 컴포넌트-월드 변환을 가져옵니다. */
	FORCEINLINE const FTransform& GetComponentTransform() const
	{
		return ComponentToWorld;
	}
private:
	/** 부모에 대한 컴포넌트의 위치 */
	UPROPERTY(EditAnywhere)
	FVector RelativeLocation;

	/** 부모에 대한 컴포넌트의 회전 */
	UPROPERTY(EditAnywhere)
	FRotator RelativeRotation;

	/**
	*   부모에 대한 컴포넌트의 비균일 스케일링
	*   스케일링은 항상 로컬 공간에서 적용됨(전단 변형 등 없음)
	*/
	UPROPERTY(EditAnywhere)
	FVector RelativeScale3D;


private:
	/** What we are currently attached to. If valid, RelativeLocation etc. are used relative to this object */
	//UPROPERTY(ReplicatedUsing = OnRep_AttachParent)
	TEnginePtr<USceneComponent> AttachParent;

private:
	/** 현재 컴포넌트의 Transform, 월드 기준 */
	FTransform ComponentToWorld;
};