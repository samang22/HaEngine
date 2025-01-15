#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
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

class APawn;
class ULevel;

UCLASS()
class ENGINE_API AActor : public UObject
{
	GENERATED_BODY()
public:
	AActor();

	/** 액터가 월드에 스폰된 후 호출됩니다. 플레이를 위한 액터 설정을 담당합니다. */
	void PostSpawnInitialize(FTransform const& SpawnTransform, AActor* InOwner, APawn* InInstigator/*, bool bRemoteOwned, bool bNoFail, bool bDeferConstruction*/, ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot);

public:
	/** Returns this actor's root component. */
	FORCEINLINE USceneComponent* GetRootComponent() const { return RootComponent.Get(); }

	/** Instigator의 값을 설정하지만 이 인스턴스에 다른 부작용을 일으키지 않습니다. */
	void SetInstigator(APawn* InInstigator);

	/**
	 * 이 액터의 소유자를 설정합니다. 주로 네트워크 복제에 사용됩니다.
	 * @param NewOwner  이 액터의 소유권을 가져가는 액터
	 */
	 //UFUNCTION(BlueprintCallable, Category = Actor)
	virtual void SetOwner(AActor* NewOwner);

	/** See if this actor is owned by TestOwner. */
	inline bool IsOwnedBy(const AActor* TestOwner) const
	{
		for (const AActor* Arg = this; Arg; Arg = Arg->Owner.Get())
		{
			if (Arg == TestOwner)
				return true;
		}
		return false;
	}

	/** 캐시된 월드 포인터에 대한 게터로, 액터가 실제로 레벨에 생성되지 않은 경우 null을 반환합니다. */
	virtual UWorld* GetWorld() const override final;

	/** Return the ULevel that this Actor is part of. */
	//UFUNCTION(BlueprintCallable, Category = Level)
	ULevel* GetLevel() const;

	/**
	* 지정된 컴포넌트를 루트 컴포넌트로 설정합니다. NewRootComponent의 소유자는 이 액터여야 합니다.
	* @return 성공하면 true를 반환합니다.
	*/
	bool SetRootComponent(USceneComponent* NewRootComponent);

	/** Components 배열의 모든 컴포넌트가 등록되었는지 확인합니다. */
	virtual void RegisterAllComponents();

	/** Components 배열의 모든 컴포넌트가 등록되기 전에 호출되며, 에디터와 게임 플레이 중 모두 호출됩니다. */
	virtual void PreRegisterAllComponents();

	/**
	 * 점진적으로 이 액터와 연관된 컴포넌트를 등록합니다. 레벨 스트리밍 중에 사용됩니다.
	 *
	 * @param NumComponentsToRegister 이번 실행에서 등록할 컴포넌트의 수입니다. 0으로 설정하면 모든 컴포넌트를 등록합니다.
	 * @return 이 액터의 모든 컴포넌트가 등록되었을 때 true를 반환합니다.
	 */
	bool IncrementalRegisterComponents(int32 NumComponentsToRegister, FRegisterComponentContext* Context = nullptr);

public:
	/**
	 * 'ComponentType' 클래스에서 파생된 모든 컴포넌트를 가져와 결과를 OutComponents 배열에 채웁니다.
	 * 메모리 할당 비용을 피하기 위해 TInlineAllocator를 사용하는 TArrays를 사용하는 것이 좋습니다.
	 * TInlineComponentArray는 이를 쉽게 하기 위해 정의되어 있습니다. 예를 들어:
	 * {
	 *     TInlineComponentArray<UPrimitiveComponent*> PrimComponents(Actor);
	 * }
	 *
	 * @param bIncludeFromChildActors true인 경우 ChildActor 컴포넌트로 재귀하여 해당 액터의 적절한 유형의 컴포넌트를 찾습니다.
	 */
	template<class ComponentType>
	void GetComponents(TArray<ComponentType*>& OutComponents/*, bool bIncludeFromChildActors = false*/) const
	{
		OutComponents.clear();

		TSubclassOf<UActorComponent> ComponentClass = ComponentType::StaticClass();
		for (TObjectPtr<UActorComponent> OwnedComponent : OwnedComponents)
		{
			if (OwnedComponent)
			{
				if (OwnedComponent->IsA(ComponentClass))
				{
					OutComponents.push_back((ComponentType*)OwnedComponent.get());
				}
			}
			else
			{
				E_LOG(Error, TEXT("OwnedComponent is nullptr"));
			}
		}
	}

	/** 컴포넌트가 올바르게 소유자의 OwnedComponents 배열에 포함되어 있는지 확인하는 유틸리티 함수 */
	bool OwnsComponent(UActorComponent* Component) const;

public:
	/**
	 * 이 액터가 BeginPlay 이벤트를 받기 전에 Tick을 허용할지 여부입니다.
	 * 일반적으로 우리는 BeginPlay 이후에만 액터를 Tick합니다; 이 설정을 통해 이러한 동작을 무효화할 수 있습니다.
	 * 이 설정이 관련되기 위해서는 이 액터가 Tick할 수 있어야 합니다.
	 */
	UPROPERTY(EditDefaultsOnly, Category = Tick)
	uint8 bAllowTickBeforeBeginPlay : 1 = false;

public:
	/**
	 * 이 액터의 소유자입니다. 주로 복제(bNetUseOwnerRelevancy 및 bOnlyRelevantToOwner) 및 가시성(PrimitiveComponent bOwnerNoSee 및 bOnlyOwnerSee)에 사용됩니다.
	 * @see SetOwner(), GetOwner()
	 */
	 //UPROPERTY(ReplicatedUsing = OnRep_Owner)
	TEnginePtr<AActor> Owner;

	/** 이 액터가 소유자인 모든 액터의 배열, 이들은 반드시 UChildActorComponent에 의해 스폰된 것은 아닙니다 */
	//UPROPERTY(Transient)
	TArray<TEnginePtr<AActor>> Children;

protected:
	/** 이 액터의 트랜스폼(위치, 회전, 스케일)을 정의하는 컴포넌트로, 모든 다른 컴포넌트는 이 컴포넌트에 어떻게든 부착되어야 합니다 */
	//UPROPERTY(BlueprintGetter = K2_GetRootComponent, Category = "Transformation")
	TEnginePtr<USceneComponent> RootComponent;

private:
	/** 이 액터가 일으킨 피해 및 기타 게임플레이 이벤트에 대한 책임이 있는 폰입니다. */
	//UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Instigator, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true), Category = Actor)
	TEnginePtr<class APawn> Instigator;

private:
	/**
	 * 이 액터가 소유한 모든 ActorComponent입니다. 액터는 많은 수의 컴포넌트를 가질 수 있으므로 Set으로 저장됩니다.
	 * @see GetComponents()
	 */
	TSet<TObjectPtr<UActorComponent>> OwnedComponents;

};  