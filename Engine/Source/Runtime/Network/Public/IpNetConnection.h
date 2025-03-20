#pragma once
#include "CoreMinimal.h"
#include "Engine/NetConnection.h"
#include "NetworkFwd.h"
#include "IpNetConnection.generated.h"

UCLASS()
class UIpNetConnection : public UNetConnection
{
    GENERATED_BODY()

public:
    virtual bool InitRemoteConnection(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext);

    FSocket* GetSocket() const { return Socket.get(); }

protected:
    virtual bool InitBase(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext);

protected:
    unique_ptr<FSocket> Socket;
};