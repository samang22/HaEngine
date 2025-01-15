#include "GameFramework/Actor.h"

/** 사용자가 자신의 네이티브 생성자에서 이를 잊어버렸을 때, 액터의 컴포넌트 계층 구조를 설정하는 유틸리티 */
static USceneComponent* FixupNativeActorComponents(AActor* Actor)
{
	USceneComponent* SceneRootComponent = Actor->GetRootComponent();
	if (SceneRootComponent == nullptr)
	{
		TArray<USceneComponent*> SceneComponents;
		Actor->GetComponents(SceneComponents);
		if (SceneComponents.size() > 0)
		{
			E_LOG(Error, TEXT("{}에는 기본적으로 씬 컴포넌트가 추가되어 있지만, 이들 중 어느 것도 액터의 RootComponent로 설정되지 않았습니다 - 임의로 하나를 선택합니다."), Actor->GetName());

			// 사용자가 네이티브 컴포넌트 중 하나를 루트로 설정하는 것을 잊어버렸다면,
			// 임의로 하나를 선택합니다 (그렇지 않으면 SCS가 자체 루트를 생성하고, 네이티브 컴포넌트를 그 아래에 중첩할 수 있습니다).
			for (USceneComponent* Component : SceneComponents)
			{
				// @TODO
				//if ((Component == nullptr) ||
				//	(Component->GetAttachParent() != nullptr) ||
				//	(Component->CreationMethod != EComponentCreationMethod::Native))
				//{
				//	continue;
				//}

				SceneRootComponent = Component;
				Actor->SetRootComponent(Component);
				break;
			}
		}
	}

	return SceneRootComponent;
}


AActor::AActor()
{

}

void AActor::PostSpawnInitialize(FTransform const& SpawnTransform, AActor* InOwner, APawn* InInstigator, ESpawnActorScaleMethod TransformScaleMethod)
{
	// 일반적인 흐름은 다음과 같습니다
	// - 액터가 기본 설정을 구성합니다.
	// - 액터가 PreInitializeComponents()를 호출합니다.
	// - 액터가 자체적으로 구성되며, 이 시점에서 컴포넌트가 완전히 조립되어야 합니다.
	// - 액터의 컴포넌트가 OnComponentCreated를 호출합니다.
	// - 액터의 컴포넌트가 InitializeComponent를 호출합니다.
	// - 모든 설정이 완료되면 액터가 PostInitializeComponents()를 호출합니다.
	//
	// 이는 지연된 스폰이든 지연되지 않은 스폰이든 동일한 순서여야 합니다.

	// 월드 정보가 스폰되기 전까지는 UWorld 접근자 함수 호출이 안전하지 않습니다.

	// Set owner.
	SetOwner(InOwner);

	// Set instigator
	SetInstigator(InInstigator);
	// 네이티브 루트 컴포넌트가 있는 경우 액터의 월드 트랜스폼을 설정합니다.
	USceneComponent* const SceneRootComponent = FixupNativeActorComponents(this);
	if (SceneRootComponent != nullptr)
	{
		_ASSERT(SceneRootComponent->GetOwner() == this);

		//// Respect any non-default transform value that the root component may have received from the archetype that's owned
		//// by the native CDO, so the final transform might not always necessarily equate to the passed-in UserSpawnTransform.
		//const FTransform RootTransform(SceneRootComponent->GetRelativeRotation(), SceneRootComponent->GetRelativeLocation(), SceneRootComponent->GetRelativeScale3D());
		//FTransform FinalRootComponentTransform = RootTransform;
		//switch (TransformScaleMethod)
		//{
		//case ESpawnActorScaleMethod::OverrideRootScale:
		//	FinalRootComponentTransform = UserSpawnTransform;
		//	break;
		//case ESpawnActorScaleMethod::MultiplyWithRoot:
		//case ESpawnActorScaleMethod::SelectDefaultAtRuntime:
		//	FinalRootComponentTransform = RootTransform * UserSpawnTransform;
		//	break;
		//}
		//SceneRootComponent->SetWorldTransform(FinalRootComponentTransform, false, nullptr, ETeleportType::ResetPhysics);
	}

	// 모든 기본(네이티브) 컴포넌트에서 OnComponentCreated를 호출합니다.
	//DispatchOnComponentsCreated(this);

	// 액터의 기본(네이티브) 컴포넌트를 등록합니다. 단, 네이티브 씬 루트가 있는 경우에만 등록합니다. 네이티브 씬 루트가 없는 경우, 
	// 네이티브 클래스 수준에서 씬 컴포넌트가 없을 수 있음을 의미합니다. 이 경우, 만약 이 인스턴스가 블루프린트 인스턴스라면, 
	// SCS 실행 후 씬 루트를 설정할 수 있을 때까지 네이티브 등록을 연기해야 합니다.
	// 참고: 이 API는 액터 인스턴스에서 PostRegisterAllComponents()를 호출합니다. 연기된 경우, PostRegisterAllComponents()는 루트가 SCS에 의해 설정될 때까지 호출되지 않습니다.
	//bHasDeferredComponentRegistration = (SceneRootComponent == nullptr && Cast<UBlueprintGeneratedClass>(GetClass()) != nullptr);
	//if (!bHasDeferredComponentRegistration && GetWorld())
	{
		RegisterAllComponents();
	}

}

void AActor::SetInstigator(APawn* InInstigator)
{
	//Instigator = InInstigator;
}

void AActor::SetOwner(AActor* NewOwner)
{
	if (Owner.Get() != NewOwner /*&& IsValidChecked(this)*/)
	{
		if (NewOwner != nullptr && NewOwner->IsOwnedBy(this))
		{
			E_LOG(Error, TEXT("SetOwner(): Failed to set '{}' owner of '{}' because it would cause an Owner loop"), NewOwner->GetName(), GetName());
			return;
		}

		// 이 액터의 부모를 지정된 액터로 설정합니다.
		if (Owner != nullptr)
		{
			// 이전 소유자의 Children 배열에서 제거합니다.
			TEnginePtr<AActor> ThisActor = this->As<AActor>();
			auto ItFind = find(Children.begin(), Children.end(), ThisActor);
			Owner->Children.erase(ItFind);
		}

		Owner = NewOwner->As<AActor>();

		if (Owner != nullptr)
		{
			// 새로운 소유자의 Children 배열에 추가합니다.
			Owner->Children.push_back(this->As<AActor>());
		}
	}
}

bool AActor::SetRootComponent(USceneComponent* NewRootComponent)
{
	if (RootComponent.Get() != NewRootComponent)
	{
		//USceneComponent* OldRootComponent = RootComponent;
		RootComponent = NewRootComponent->As<USceneComponent>();

		// Notify new root first, as it probably has no delegate on it.
		/*if (NewRootComponent)
		{
			NewRootComponent->NotifyIsRootComponentChanged(true);
		}

		if (OldRootComponent)
		{
			OldRootComponent->NotifyIsRootComponentChanged(false);
		}*/

		return true;
	}

	return false;
}

void AActor::RegisterAllComponents()
{
}
