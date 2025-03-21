#pragma once
#include "CoreMinimal.h"
#include "Engine/Player.h"
#include "Engine/EngineBaseTypes.h"
#include "NetConnection.generated.h"


class UNetDriver;

enum EConnectionState : uint8
{
    USOCK_Invalid = 0, // Connection is invalid, possibly uninitialized.
    USOCK_Closed = 1, // Connection permanently closed.
    USOCK_Pending = 2, // Accept는 했으나, 아직 아무런 Packet도 받지 못한
    USOCK_Open = 3, // Connection is open.
};


UCLASS()
class ENGINE_API UNetConnection : public UPlayer
{
    GENERATED_BODY()

protected:
    UNetDriver* NetDriver = nullptr;
    FURL URL;
    EConnectionState ConnectionState = EConnectionState::USOCK_Invalid;

public:
    /**
     * 원격 소스에서 이 연결 인스턴스를 초기화합니다.
     *
     * @param InDriver 이 연결과 관련된 네트 드라이버
     * @param InSocket 이 연결과 관련된 소켓
     * @param InURL 초기화할 URL
     * @param InRemoteAddr 이 연결의 원격 주소
     * @param InState 이 연결에 대해 시작할 연결 상태
     * @param InMaxPacket 전송에 사용할 최대 패킷 크기
     * @param InPacketOverhead 이 연결 유형에 대한 패킷 오버헤드
     */
    virtual bool InitRemoteConnection(UNetDriver* InDriver, const FURL& InURL);

    void SetConnectionState(EConnectionState NewState) { ConnectionState = NewState; }
    
    virtual void OnAccepted() {}
    virtual void ReceivedRawPacket(uint64 InRecvSize) {}

protected:
    virtual bool InitBase(UNetDriver* InDriver, const FURL& InURL);

};