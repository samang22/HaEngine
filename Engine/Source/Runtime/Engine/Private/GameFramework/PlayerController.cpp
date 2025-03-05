#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Player.h"

void APlayerController::SetPlayer(UPlayer* InPlayer)
{
    // Set the viewport.
    Player = InPlayer;
    InPlayer->PlayerController = this;

    // 로컬 플레이어에만 해당되는 초기화
    ULocalPlayer* LP = dynamic_cast<ULocalPlayer*>(InPlayer);
    if (LP != NULL)
    {
        // 클라이언트는 이것을 로컬로 표시해야 합니다 (서버는 이미 생성 시 알았습니다)
        // SetAsLocalPlayerController();
        //LP->InitOnlineSession();
        InitInputSystem();
    }
    else
    {
        _ASSERT(false);
    }
}

void APlayerController::InitInputSystem()
{
    /*if (PlayerInput == nullptr)
    {
        const UClass* OverrideClass = OverridePlayerInputClass.Get();

        PlayerInput = NewObject<UPlayerInput>(this, OverrideClass ? OverrideClass : UInputSettings::GetDefaultPlayerInputClass());
    }*/

    SetupInputComponent();
}

void APlayerController::SetupInputComponent()
{
    // 서브클래스는 다른 InputComponent 클래스를 생성할 수 있지만 기본 바인딩을 여전히 원할 수 있습니다
    if (InputComponent == NULL)
    {
        //InputComponent = NewObject<UInputComponent>(this, UInputSettings::GetDefaultInputComponentClass(), TEXT("PC_InputComponent0"));
        InputComponent = NewObject<UInputComponent>(this, UInputComponent::StaticClass(), TEXT("PC_InputComponent0"));
        InputComponent->RegisterComponent();
    }

    /*if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
    {
        InputComponent->bBlockInput = bBlockInput;
        UInputDelegateBinding::BindInputDelegatesWithSubojects(this, InputComponent);
    }*/
}