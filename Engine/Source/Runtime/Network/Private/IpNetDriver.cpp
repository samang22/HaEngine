#include "IpNetDriver.h"
#include "NetworkTypes.h"
#include "IpConnection.h"
#include <boost/asio/ip/tcp.hpp>
#include "Net/DataBunch.h"

UIpNetDriver::UIpNetDriver()
{
	SetNetConnectionClass(UIpConnection::StaticClass());
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

bool UIpNetDriver::InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error)
{
    if (!Super::InitConnect(InNotify, ConnectURL, Error))
    {
        return false;
    }

    ServerConnection = NewObject<UIpConnection>(this, NetConnectionClass);
    bool bResult = ServerConnection->InitRemoteConnection(this, URL, Context);

    return bResult;
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
    TObjectPtr<UIpConnection> NetConnection = NewObject<UIpConnection>(this, NetConnectionClass);
    NetConnection->InitRemoteConnection(this, URL, Context);
    Backlogs.emplace(NetConnection.get(), NetConnection);

    Acceptor->async_accept(*NetConnection->GetSocket(),
        [this, NetConnection](const boost::system::error_code& Error)
        {
            Backlogs.erase(NetConnection.get());

            StartAceept();

            if (Error)
            {
                E_LOG(Error, TEXT("Accept failed: {}"), ANSI_TO_TCHAR(Error.message()));
                return;
            }

            NetConnection->OnAccepted();

            E_LOG(Log, TEXT("[Accepted]: {}:{}"), NetConnection->GetRemoteIP(), NetConnection->GetRemotePort());

            PendingConnections.emplace(NetConnection.get(), NetConnection);

            boost::asio::async_read(*NetConnection->GetSocket(),
                boost::asio::buffer(NetConnection->GetRecvBuffer().data(), sizeof(FPacketHeader)),
                [this, NetConnection](const boost::system::error_code& Error, uint64 InRecvSize)
                {
                    if (Error)
                    {
                        PendingConnections.erase(NetConnection.get());
                        E_LOG(Log, TEXT("[Read Failed]: {}, {}:{}"), ANSI_TO_TCHAR(Error.message()), NetConnection->GetRemoteIP(), NetConnection->GetRemotePort());
                        return;
                    }

                    NetConnection->ReceivedRawPacket(InRecvSize);
                }
            );
        }
    );
}
