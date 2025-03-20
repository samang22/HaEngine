#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"

bool UNetConnection::InitRemoteConnection(UNetDriver* InDriver, const FURL& InURL)
{
    return InitBase(InDriver, InURL);
}

bool UNetConnection::InitBase(UNetDriver* InDriver, const FURL& InURL)
{
    NetDriver = InDriver;
    URL = InURL;
    SetConnectionState(EConnectionState::USOCK_Invalid);
    return true;
}