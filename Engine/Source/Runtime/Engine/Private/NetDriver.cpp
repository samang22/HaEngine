#include "NetDriver.h"
#include "Engine/NetConnection.h"

UNetDriver::UNetDriver()
{
}

UNetDriver::~UNetDriver()
{
}


bool UNetDriver::InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
    bool bSuccess = InitConnectionClass();

    //if (!bInitAsClient)
    //{
    //    // ConnectionlessHandler.Reset();
    //}

    Notify = InNotify;

    return bSuccess;
}

bool UNetDriver::InitConnectionClass(void)
{
    if (NetConnectionClass == NULL /*&& NetConnectionClassName != TEXT("")*/)
    {
        _ASSERT(false);
        NetConnectionClass = UNetConnection::StaticClass();
        /*NetConnectionClass = LoadClass<UNetConnection>(NULL, *NetConnectionClassName, NULL, LOAD_None, NULL);
        if (NetConnectionClass == NULL)
        {
            UE_LOG(LogNet, Error, TEXT("Failed to load class '%s'"), *NetConnectionClassName);
        }*/
    }

    return true;
}