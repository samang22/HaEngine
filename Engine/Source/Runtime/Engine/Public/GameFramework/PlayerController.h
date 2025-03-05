#pragma once
#include "GameFramework/Controller.h"
#include "PlayerController.generated.h"

class APawn;
class UInputComponent;
class UPlayer;
class UPrimitiveComponent;

/**
 * PlayerController는 인간 플레이어가 Pawns를 제어하는 데 사용됩니다.
 *
 * ControlRotation (GetControlRotation()을 통해 액세스)은 제어된 Pawn의 조준 방향을 결정합니다.
 *
 * 네트워크 게임에서, PlayerControllers는 모든 플레이어가 제어하는 Pawn에 대해 서버에 존재하며,
 * 또한 제어하는 클라이언트의 기기에서도 존재합니다. 다른 네트워크의 원격 플레이어가 제어하는 Pawn에 대해서는
 * 클라이언트 기기에 존재하지 않습니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Framework/Controller/PlayerController/
 */
UCLASS(/*config = Game, BlueprintType, Blueprintable, meta = (ShortTooltip = "Player Controller는 플레이어가 사용하는 Pawn을 제어하는 역할을 하는 액터입니다."), MinimalAPI*/)
class ENGINE_API APlayerController : public AController//, public IWorldPartitionStreamingSourceProvider
{
    GENERATED_BODY()

public:
    /** 이 PlayerController에 새 UPlayer를 연결합니다. */
    virtual void SetPlayer(UPlayer* InPlayer);

    /** 이 PlayerController와 연결된 UPlayer입니다. 로컬 플레이어일 수도 있고 네트워크 연결일 수도 있습니다. */
    //UPROPERTY()
    //TObjectPtr<UPlayer> Player;
    UPlayer* Player = nullptr;

public:
    /**
     * 적절한 클래스의 PlayerInput을 생성합니다.
     * 로컬 플레이어에 속한 플레이어 컨트롤러에 대해서만 호출됩니다.
     */
    virtual void InitInputSystem();

protected:
    /** PlayerController가 사용자 정의 입력 바인딩을 설정할 수 있도록 합니다. */
    virtual void SetupInputComponent();

public:
    /** 입력이 활성화된 경우, 이 액터의 입력을 처리하는 컴포넌트입니다. */
    //UPROPERTY(/*DuplicateTransient*/)
    TObjectPtr<class UInputComponent> InputComponent;
};