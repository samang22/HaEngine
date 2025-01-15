#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"


ENGINE_API void FRegisterComponentContext::SendRenderDynamicData(FRegisterComponentContext* Context, UPrimitiveComponent* PrimitiveComponent)
{
}

void FRegisterComponentContext::Process()
{
}

UActorComponent::UActorComponent()
{
    OwnerPrivate = GetTypedOuter<AActor>();
}

AActor* UActorComponent::GetOwner() const
{
    return GetActorOwnerNoninline();
}

void UActorComponent::RegisterComponentWithWorld(UWorld* InWorld, FRegisterComponentContext* Context)
{
	if (!this)
	{
		E_LOG(Log, TEXT("RegisterComponentWithWorld: ({}) Trying to register component with IsValid() == false. Aborting."), GetName());
		return;
	}

	// If the component was already registered, do nothing
	if (IsRegistered())
	{
		E_LOG(Log, TEXT("RegisterComponentWithWorld: ({}) Already registered. Aborting."), GetName());
		return;
	}

	if (InWorld == nullptr)
	{
		//UE_LOG(LogActorComponent, Log, TEXT("RegisterComponentWithWorld: (%s) NULL InWorld specified. Aborting."), *GetPathName());
		return;
	}

	// 등록되지 않은 경우, 씬을 가져서는 안 됩니다.
	_ASSERT(WorldPrivate == nullptr);// , TEXT("%s"), * GetFullName());

	AActor* MyOwner = GetOwner();
	_ASSERT(MyOwner == nullptr || MyOwner->OwnsComponent(this));

	if (!bHasBeenCreated)
	{
		OnComponentCreated();
	}
}

void UActorComponent::OnComponentCreated()
{
	_ASSERT(!bHasBeenCreated);
	bHasBeenCreated = true;
}

AActor* UActorComponent::GetActorOwnerNoninline() const
{
    _ASSERT(OwnerPrivate);
    return OwnerPrivate;
}

