#pragma once
#include "Player.h"
#include "LocalPlayer.generated.h"

              
/**
 *  현재 클라이언트/리스닝 서버에서 활성화된 각 플레이어는 LocalPlayer를 가지고 있습니다.
 *  이 플레이어는 맵 간에 활성 상태를 유지하며, 스플릿스크린/협동 모드의 경우 여러 개가 생성될 수 있습니다.
 *  전용 서버에서는 0개가 생성됩니다.
 */
UCLASS()
class ENGINE_API ULocalPlayer : public UPlayer
{
    GENERATED_BODY()

public:
    /**
    * 내부 설정을 위해 생성 시 호출됩니다
    */
    virtual void PlayerAdded(class UGameViewportClient* InViewportClient/*, FPlatformUserId InUserId*/);

    /**
     * 이 플레이어를 위한 액터를 생성합니다.
     * @param URL - 플레이어가 접속한 URL입니다.
     * @param OutError - 오류가 발생한 경우, 오류 설명을 반환합니다.
     * @param InWorld - 플레이 액터를 스폰할 월드입니다.
     * @return 오류가 발생한 경우 false를 반환하고, 플레이 액터가 성공적으로 스폰된 경우 true를 반환합니다.
     */
    virtual void SpawnPlayActor(/*const FString& URL, FString& OutError,*/ UWorld* InWorld);

protected:
    /** 이 플레이어의 뷰를 포함하는 기본 뷰포트입니다. */
    //UPROPERTY()
    class UGameViewportClient* ViewportClient = nullptr;

};