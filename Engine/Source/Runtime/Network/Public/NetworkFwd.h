#pragma once
#include "CoreMinimal.h"
#include <boost/asio/ip/tcp.hpp>

namespace boost
{
    namespace asio
    {
        class io_context;
    } // namespace asio
} // namespace boost


using namespace boost::asio::ip;
using FAcceptor = boost::asio::ip::tcp::acceptor;
using FIOContext = boost::asio::io_context;
using FSocket = boost::asio::ip::tcp::socket;
using FEndPoint = boost::asio::ip::tcp::endpoint;