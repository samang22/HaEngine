#include "IpNetDriver.h"
#include "NetworkTypes.h"
#include "IpNetConnection.h"
#include <boost/asio/ip/tcp.hpp>

UIpNetDriver::UIpNetDriver()
{
	SetNetConnectionClass(UIpNetConnection::StaticClass());
}

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
	if (!Super::InitListen(InNotify, ListenURL, bReuseAddressAndPort, Error)) 
	{
		return false;
	}

	Acceptor = make_shared<FAcceptor>(Context,
		tcp::endpoint(tcp::v4(), (port_type)ListenURL.Port), bReuseAddressAndPort);

	const int32 AsyncAceeptCount = 10;
	for (int32 i = 0; i < AsyncAceeptCount; ++i)
	{
		StartAceept();
	}

    return true;
}

void UIpNetDriver::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    uint32 PollCount = 0;
    const uint32 MaxPollCountPerTick = 1000;
    boost::system::error_code ErrorCode;
    while (uint32 n = Context.poll_one(ErrorCode))
    {
        if (ErrorCode)
        {
            E_LOG(Error, TEXT("Context poll error: {}"), ANSI_TO_TCHAR(ErrorCode.message()));
            break;
        }
        PollCount += n;
        if (PollCount >= MaxPollCountPerTick)
        {
            break;
        }
    }
}

void UIpNetDriver::StartAceept()
{
    TObjectPtr<UIpNetConnection> NetConnection = NewObject<UIpNetConnection>(this, NetConnectionClass);
    NetConnection->InitRemoteConnection(this, URL, Context);
    Backlog.emplace(NetConnection.get(), NetConnection);

    Acceptor->async_accept(*NetConnection->GetSocket(),
        [](const boost::system::error_code& Error)
        {
            E_LOG(Log, TEXT("Accepted!"));
        }
    );
}
