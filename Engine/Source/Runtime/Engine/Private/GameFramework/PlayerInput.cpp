#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"

TArray<FInputAxisKeyMapping> UPlayerInput::EngineDefinedAxisMappings;

UPlayerInput::UPlayerInput()
{
}

void UPlayerInput::ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused)
{
    APlayerController* PlayerController = GetOuterAPlayerController();
    if (PlayerController)
    {
        //PlayerController->PreProcessInput(DeltaTime, bGamePaused);
    }

    // EvaluateInputDelegates
    {
        DirectX::Keyboard::State KeyboardState = DirectX::Keyboard::Get().GetState();
        for (UInputComponent* const InputComponent : InputComponentStack)
        {
            if (!InputComponent->AxisBindings.empty())
            {
                for (FInputAxisBinding& ItAxisBinding : InputComponent->AxisBindings)
                {
                    // 동일한 이름의 AxisKeyMapping 수집
                    TArray<FInputAxisKeyMapping> Results;
                    Results.reserve(EngineDefinedAxisMappings.size());
                    std::for_each(EngineDefinedAxisMappings.begin(), EngineDefinedAxisMappings.end(),
                        [&ItAxisBinding, &Results](FInputAxisKeyMapping& Mapping)
                        {
                            if (ItAxisBinding.AxisName == Mapping.AxisName)
                            {
                                Results.push_back(Mapping);
                            }
                        });

                    float Scale = 0.f;
                    for (auto& ItFind : Results)
                    {
                        const bool bKeyDown = KeyboardState.IsKeyDown(ItFind.Key);
                        if (bKeyDown)
                        {
                            Scale += ItFind.Scale;
                        }
                    }
                    ItAxisBinding.AxisDelegate.Broadcast(Scale);
                }
            }
        }
    }

    if (PlayerController)
    {
        //PlayerController->PostProcessInput(DeltaTime, bGamePaused);
    }
}

APlayerController* UPlayerInput::GetOuterAPlayerController() const
{
    return dynamic_cast<APlayerController*>(GetOuter());
}

void UPlayerInput::AddEngineDefinedAxisMapping(const FInputAxisKeyMapping& AxisMapping)
{
    auto It = find_if(EngineDefinedAxisMappings.begin(), EngineDefinedAxisMappings.end(),
        [&](const FInputAxisKeyMapping& Mapping)
        {
            return Mapping == AxisMapping;
        });
    if (It == EngineDefinedAxisMappings.end())
    {
        EngineDefinedAxisMappings.push_back(AxisMapping);
    }
    else
    {
        E_LOG(Error, TEXT("중복 FInputAxisKeyMapping 입니다."));
    }
}