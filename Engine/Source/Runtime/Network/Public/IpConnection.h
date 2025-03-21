#pragma once
#include "CoreMinimal.h"
#include "Engine/NetConnection.h"
#include "NetworkFwd.h"
#include "IpConnection.generated.h"

//
UCLASS()
class NETWORK_API UIpConnection : public UNetConnection
{
    GENERATED_BODY()

public:
    virtual bool InitRemoteConnection(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext);
    virtual void OnAccepted() override;
    virtual void ReceivedRawPacket(uint64 InRecvSize) override; // 수신한 데이터는 RecvBuffer에 있습니다.


public:
    FSocket* GetSocket() const { return Socket.get(); }
    const FEndPoint& GetRemoteEndPoint() const { return RemoteEndPoint; }
    const FString& GetRemoteIP() const { return RemoteIP; }
    const uint16 GetRemotePort() const { return RemotePort; }

    void SetRecvBuffer(const uint32 InRecvBufferSize) { RecvBufferSize = InRecvBufferSize; }
    TArray<uint8>& GetRecvBuffer() { return RecvBuffer; }

protected:
    virtual bool InitBase(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext);

protected:
    // 클라/서버 간 통신을 위한 연결 설정
    // 두 프로세스 간 데이터 송수신
    unique_ptr<FSocket> Socket;
    // IP와 Port로 구성되는 주소
    FEndPoint RemoteEndPoint;
    FString RemoteIP;
    uint16 RemotePort = 0;
    uint32 RecvBufferSize = 4096;
    TArray<uint8> RecvBuffer;
};