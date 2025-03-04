#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "GameInstance.generated.h"

class ULocalPlayer;
class UWorld;

/**
 * GameInstance: 실행 중인 게임의 인스턴스를 위한 고수준 관리자 객체.
 * 게임 생성 시 생성되며, 게임 인스턴스가 종료될 때까지 파괴되지 않습니다.
 * 독립 실행형 게임으로 실행 중인 경우, 하나의 GameInstance가 존재합니다.
 * PIE(에디터 내 플레이)에서 실행 중인 경우, PIE 인스턴스당 하나의 GameInstance가 생성됩니다.
 */

UCLASS()
class ENGINE_API UGameInstance : public UObject
{
    GENERATED_BODY()
public:
    UGameInstance();

    class UEngine* GetEngine() const;
    virtual UWorld* GetWorld() const override { return World; }

#if WITH_EDITOR
    /* 게임의 PIE 인스턴스를 위해 게임 인스턴스를 초기화하는데 호출됩니다 */
    virtual void InitializeForPlayInEditor();

    /* 에디터에서 Play/Simulate 실행 시 실제로 게임을 시작하는 함수입니다 */
    virtual void StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer);
#endif

protected:
    UWorld* World = nullptr;

public:
    /** 주어진 맵 URL을 위해 게임 모드를 생성하는 호출 */
    virtual class AGameModeBase* CreateGameModeForURL(FURL InURL, UWorld* InWorld);
};