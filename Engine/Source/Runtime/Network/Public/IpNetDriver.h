#pragma once
#include "NetDriver.h"
#include "IpNetDriver.generated.h"

UCLASS()
class NETWORK_API UIpNetDriver : public UNetDriver
{
    GENERATED_BODY()
public:
    virtual bool InitListen(class FNetworkNotify* InNotify, FURL& ListenURL, bool bReuseAddressAndPort, FString& Error);
};