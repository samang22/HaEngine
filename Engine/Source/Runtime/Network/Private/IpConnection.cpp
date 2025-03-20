#include "IpConnection.h"
#include "Engine/NetDriver.h"
#include "NetworkTypes.h"

bool UIpConnection::InitRemoteConnection(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext)
{
	bool bResult = InitBase(InDriver, InURL, InContext);
	if (bResult == false)
	{
		return bResult;
	}

    const bool bServer = InDriver->IsServer();
    if (!bServer)
    {
        boost::asio::ip::tcp::endpoint EndPoint(boost::asio::ip::address::from_string(TCHAR_TO_ANSI(InURL.Host)), URL.Port);
        tcp::resolver Resolver(InContext);
        boost::system::error_code ErrorCode;
        boost::asio::connect(*GetSocket(), Resolver.resolve(EndPoint), ErrorCode);

        if (ErrorCode)
        {
            E_LOG(Error, TEXT("IpNetConnection Failed to connect: {}"), ANSI_TO_TCHAR(ErrorCode.message()));
            return false;
        }
    }

    return true;
}

bool UIpConnection::InitBase(UNetDriver* InDriver, const FURL& InURL, FIOContext& InContext)
{
	bool bResult = Super::InitBase(InDriver, InURL);

	Socket = make_unique<FSocket>(InContext);

	return bResult;
}
