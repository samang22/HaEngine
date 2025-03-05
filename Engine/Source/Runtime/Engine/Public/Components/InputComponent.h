#pragma once
#include "Components/ActorComponent.h"
#include "InputComponent.generated.h"

/**
 * 입력 바인딩을 위한 액터 컴포넌트를 구현합니다.
 *
 * 입력 컴포넌트는 다양한 형태의 입력 이벤트를 대리 함수에 바인딩할 수 있도록 하는 일시적인 컴포넌트입니다.
 * 입력 컴포넌트는 PlayerController가 관리하는 스택에서 처리되고, PlayerInput에 의해 처리됩니다.
 * 각 바인딩은 입력 이벤트를 소비하여 입력 스택의 다른 컴포넌트가 입력을 처리하지 못하게 합니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Input/index.html
 */
UCLASS(/*NotBlueprintable, transient, config = Input, hidecategories = (Activation, "Components|Activation"), MinimalAPI*/)
class UInputComponent : public UActorComponent
{
    GENERATED_BODY()
};