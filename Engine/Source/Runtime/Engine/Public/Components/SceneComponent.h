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

private:
	/** What we are currently attached to. If valid, RelativeLocation etc. are used relative to this object */
	//UPROPERTY(ReplicatedUsing = OnRep_AttachParent)
	TEnginePtr<USceneComponent> AttachParent;
};