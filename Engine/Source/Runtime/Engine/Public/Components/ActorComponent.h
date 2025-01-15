#pragma once
#include "CoreMinimal.h"
#include "ActorComponent.generated.h"

class AActor;
class UWorld;
class UPrimitiveComponent;

class FRegisterComponentContext
{
public:
	FRegisterComponentContext(UWorld* InWorld)
		: World(InWorld)
	{
	}

	void AddPrimitive(UPrimitiveComponent* PrimitiveComponent)
	{
		//_ASSERT(!AddPrimitiveBatches.contain(PrimitiveComponent));
		AddPrimitiveBatches.emplace_back(PrimitiveComponent);
	}

	//void AddSendRenderDynamicData(UPrimitiveComponent* PrimitiveComponent)
	//{
	//	//checkSlow(!SendRenderDynamicDataPrimitives.Contains(PrimitiveComponent));
	//	SendRenderDynamicDataPrimitives.Add(PrimitiveComponent);
	//}

	ENGINE_API static void SendRenderDynamicData(FRegisterComponentContext* Context, UPrimitiveComponent* PrimitiveComponent);

	int32 Count() const { return AddPrimitiveBatches.size(); }
	void Process();

private:
	UWorld* World;
	TArray<UPrimitiveComponent*> AddPrimitiveBatches;
	//TArray<UPrimitiveComponent*> SendRenderDynamicDataPrimitives;
};


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

	/** 이 컴포넌트가 현재 등록되어 있는지 확인합니다. */
	FORCEINLINE bool IsRegistered() const { return bRegistered; }

	/**
	 * 컴포넌트를 특정 월드에 등록하여 시각적/물리적 상태를 생성합니다.
	 * @param InWorld - 컴포넌트를 등록할 월드입니다.
	 */
	void RegisterComponentWithWorld(UWorld* InWorld, FRegisterComponentContext* Context = nullptr);

	/** 컴포넌트가 생성될 때 호출됩니다(로드되는 것이 아님). 이는 에디터에서나 게임 플레이 중에 발생할 수 있습니다. */
	virtual void OnComponentCreated();

protected:
	/**
	 * 이 ActorComponent가 현재 씬에 등록되어 있는지 여부를 나타냅니다.
	 */
	uint8 bRegistered : 1 = false;

private:
	AActor* GetActorOwnerNoninline() const;

private:
	/** Cached pointer to owning actor */
	mutable AActor* OwnerPrivate;

	/**
	 * 이 컴포넌트가 현재 등록된 월드에 대한 포인터입니다.
	 * 컴포넌트가 등록된 경우에만 NULL이 아닙니다.
	 */
	UWorld* WorldPrivate = nullptr;

private:
	/** OnCreatedComponent가 호출되었지만 OnDestroyedComponent는 아직 호출되지 않았음을 나타냅니다. */
	uint8 bHasBeenCreated : 1 = false;
};