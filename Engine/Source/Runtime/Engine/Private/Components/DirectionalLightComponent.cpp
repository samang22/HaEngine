#include "Components/DirectionalLightComponent.h"

FVector4D UDirectionalLightComponent::GetLightPosition() const
{
    return FVector4D();
}

FLightSceneProxy* UDirectionalLightComponent::CreateSceneProxy()
{
    return nullptr;
}