#pragma once
#include "Engine/NetDriver.h"
#include "NetworkFwd.h"
#include "IpNetDriver.generated.h"

class UIpNetConnection;

UCLASS()
class NETWORK_API UIpNetDriver : public UNetDriver
{
	GENERATED_BODY()
public:
    UIpNetDriver();
    virtual bool InitBase(bool bInitAsClient, class FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error) override;
    virtual bool InitListen(class FNetworkNotify* InNotify, FURL& ListenURL, bool bReuseAddressAndPort, FString& Error) override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    void StartAceept();

protected:
    // - socket 생성
    // - bind(포트와 IP를 socket에 연결)
    // - Listen
    // - Accept
    // 처리를 하는 boost asio class이다
    shared_ptr<FAcceptor> Acceptor;
    FIOContext Context;

    map<UIpNetConnection*, TObjectPtr<UIpNetConnection>> Backlog;
};