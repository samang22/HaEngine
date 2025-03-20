#include "IpNetDriver.h"

bool UIpNetDriver::InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& LocalURL, bool bReuseAddressAndPort, FString& Error)
{
	if (!Super::InitBase(false, InNotify, LocalURL, bReuseAddressAndPort, Error))
	{
		E_LOG(Warning, TEXT("Failed to init net driver ListenURL: {}: {}"), LocalURL.ToString(), Error);
		return false;
	}

    return true;
}

bool UIpNetDriver::InitListen(FNetworkNotify* InNotify, FURL& ListenURL, bool bReuseAddressAndPort, FString& Error)
{
	if (!InitBase(false, InNotify, ListenURL, bReuseAddressAndPort, Error))
	{
		return false;
	}

    return true;
}
