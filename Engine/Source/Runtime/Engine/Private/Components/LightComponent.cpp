#include "Components/LightComponent.h"
#include "Engine/World.h"
#include "SceneInterface.h"

FVector ULightComponent::GetDirection() const
{
    _ASSERT(false); // 구현 해야함
    return FVector();
}

void ULightComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
    Super::CreateRenderState_Concurrent(Context);

    UWorld* World = GetWorld();
    //World->Scene->AddLight(this);
}