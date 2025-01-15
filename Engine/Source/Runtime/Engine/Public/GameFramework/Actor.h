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
};  