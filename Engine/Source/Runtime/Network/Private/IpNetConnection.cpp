#include "IpNetConnection.h"

bool UIpNetConnection::InitRemoteConnection(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext)
{
	bool bResult = InitBase(InDriver, InURL, InContext);

	return bResult;
}

bool UIpNetConnection::InitBase(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext)
{
	bool bResult = Super::InitBase(InDriver, InURL);

	Socket = make_unique<FSocket>(InContext);

	return bResult;
}
