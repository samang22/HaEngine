#include "Engine/NetDriver.h"
#include "Engine/NetConnection.h"


UNetDriver::UNetDriver()
{
}

UNetDriver::~UNetDriver()
{
}

void UNetDriver::SetNetConnectionClass(TSubclassOf<UNetConnection> InClass)
{
    NetConnectionClass = InClass;
}


bool UNetDriver::InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& InURL, bool bReuseAddressAndPort, FString& Error)
{
    bool bSuccess = InitConnectionClass();

    //if (!bInitAsClient)
    //{
    //    // ConnectionlessHandler.Reset();
    //}

    Notify = InNotify;
    URL = InURL;
    return bSuccess;
}

bool UNetDriver::InitConnectionClass()
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