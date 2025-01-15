#include "GameFramework/Actor.h"

/** Util that sets up the actor's component hierarchy (when users forget to do so, in their native ctor) */
static USceneComponent* FixupNativeActorComponents(AActor* Actor)
{
	USceneComponent* SceneRootComponent = Actor->GetRootComponent();
	if (SceneRootComponent == nullptr)
	{
		TArray<USceneComponent*> SceneComponents;
		//Actor->GetComponents(SceneComponents);
		//if (SceneComponents.Num() > 0)
		//{
		//	UE_LOG(LogActor, Warning, TEXT("%s has natively added scene component(s), but none of them were set as the actor's RootComponent - picking one arbitrarily"), *Actor->GetFullName());

		//	// if the user forgot to set one of their native components as the root, 
		//	// we arbitrarily pick one for them (otherwise the SCS could attempt to 
		//	// create its own root, and nest native components under it)
		//	for (USceneComponent* Component : SceneComponents)
		//	{
		//		if ((Component == nullptr) ||
		//			(Component->GetAttachParent() != nullptr) ||
		//			(Component->CreationMethod != EComponentCreationMethod::Native))
		//		{
		//			continue;
		//		}

		//		SceneRootComponent = Component;
		//		Actor->SetRootComponent(Component);
		//		break;
		//	}
		//}
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
			auto ItFind = find(Children.begin(), Children.end(), this);
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
