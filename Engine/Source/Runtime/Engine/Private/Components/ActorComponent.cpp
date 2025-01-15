#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"

UActorComponent::UActorComponent()
{
    OwnerPrivate = GetTypedOuter<AActor>();
}

AActor* UActorComponent::GetOwner() const
{
    return GetActorOwnerNoninline();
}

AActor* UActorComponent::GetActorOwnerNoninline() const
{
    _ASSERT(OwnerPrivate);
    return OwnerPrivate;
}
