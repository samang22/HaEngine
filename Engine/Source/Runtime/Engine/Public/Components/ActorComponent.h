#pragma once
#include "CoreMinimal.h"
#include "ActorComponent.generated.h"

class AActor;

/**
 * ActorComponent는 다양한 유형의 액터에 추가될 수 있는 재사용 가능한 동작을 정의하는 컴포넌트의 기본 클래스입니다.
 * Transform을 가진 ActorComponent는 SceneComponent로 알려져 있으며, 렌더링할 수 있는 ActorComponent는 PrimitiveComponent입니다.
 *
 * @see [ActorComponent](https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html#actorcomponents)
 * @see USceneComponent
 * @see UPrimitiveComponent
 */
UCLASS()
class ENGINE_API UActorComponent : public UObject
{
	GENERATED_BODY()
public:
	UActorComponent();

public:
	/** Outer 체인을 따라 이 컴포넌트를 '소유'하는 AActor를 가져옵니다. */
	//UFUNCTION(BlueprintCallable, Category = "Components", meta = (Keywords = "Actor Owning Parent"))
	AActor* GetOwner() const;

private:
	AActor* GetActorOwnerNoninline() const;

private:
	/** Cached pointer to owning actor */
	mutable AActor* OwnerPrivate;

};