#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "NetDriver.generated.h"
//
class UNetConnection;

UCLASS()
class ENGINE_API UNetDriver : public UObject
{
    GENERATED_BODY()

public:
    UNetDriver();
    virtual ~UNetDriver();

    virtual void Tick(float DeltaSeconds) {}

public:
    void SetNetConnectionClass(TSubclassOf<UNetConnection> InClass);
    /**
     * 서버와 클라이언트 연결 설정 간의 공통 초기화
     *
     * @param bInitAsClient 클라이언트로 초기화할지 서버로 초기화할지 여부
     * @param InNotify 네트 드라이버와 연관된 알림 객체
     * @param URL 목적지 URL
     * @param bReuseAddressAndPort 동일한 주소/포트에 여러 소켓을 바인딩할 수 있는지 여부
     * @param Error 실패 시 오류 문자열을 포함하는 출력 매개변수
     *
     * @return 성공하면 true, 그렇지 않으면 false (오류 메시지는 Error 매개변수 확인)
     */
    virtual bool InitBase(bool bInitAsClient, class FNetworkNotify* InNotify, const FURL& InURL, bool bReuseAddressAndPort, FString& Error);

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
    virtual bool InitListen(class FNetworkNotify* InNotify, FURL& ListenURL, bool bReuseAddressAndPort, FString& Error)
    {
        bServer = true;
        return InitBase(!bServer, InNotify, ListenURL, bReuseAddressAndPort, Error);
    }


    /**
    * 새로운 연결에 사용할 NetConnection 클래스를 초기화합니다.
    */
    virtual bool InitConnectionClass();

public:
    /** 사용할 NetConnection Class */
    TSubclassOf<UNetConnection> NetConnectionClass;

    /**
     * 네트워크 상태를 다른 객체(예: World, FNetworkNotify를 구현하는 다른 객체)에 전달하기 위한 인터페이스
     */
    class FNetworkNotify* Notify;
    FURL URL;
    bool bServer = false;

};