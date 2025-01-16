#include "Components/SceneComponent.h"

USceneComponent::USceneComponent()
{
}

USceneComponent* USceneComponent::GetAttachParent() const
{
    return AttachParent;
}
