#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "NetDriver.generated.h"

UCLASS()
class ENGINE_API UNetDriver : public UObject
{
    GENERATED_BODY()

public:
    UNetDriver();
    virtual ~UNetDriver();

public:
    /**
     * 서버 모드에서 네트워크 드라이버를 초기화합니다 (리스너)
     *
     * @param InNotify 네트 드라이버와 연관된 알림 객체
     * @param ListenURL 이 리스너의 연결 URL
     * @param bReuseAddressAndPort 동일한 주소/포트에 여러 소켓을 바인딩할 수 있는지 여부
     * @param Error 생성된 오류 메시지가 저장될 out 매개변수
     *
     * @return 성공하면 true, 그렇지 않으면 false (오류 메시지는 Error 매개변수 확인)
     */
    virtual bool InitListen(class FNetworkNotify* InNotify, FURL& ListenURL, bool bReuseAddressAndPort, FString& Error) { return false; }
};