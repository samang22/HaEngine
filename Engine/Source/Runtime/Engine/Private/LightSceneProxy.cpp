#include "LightSceneProxy.h"

FLightSceneProxy::FLightSceneProxy(ULightComponent* InLightComponent)
    : LightComponent(InLightComponent)
{
}

FLightSceneProxy::~FLightSceneProxy() = default;