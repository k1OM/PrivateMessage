
#include "AsyncNetworkClient.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

AsyncNetworkClient::AsyncNetworkClient(asio::io_context& context)
    : resolver(context),
      socket(context),
      connectionTimer(context),
      header{},
      payload{},
      outgoingMessages{},
      connected(false),
      stopped(false),
      connectionState(ConnectionState::Idle)
{
}

void AsyncNetworkClient::setOnConnect(ConnectCallback callback)
{
    onConnect = std::move(callback);
}

void AsyncNetworkClient::setOnMessage(MessageCallback callback)
{
    onMessage = std::move(callback);
}

void AsyncNetworkClient::setOnDisconnect(DisconnectCallback callback)
{
    onDisconnect = std::move(callback);
}


void AsyncNetworkClient::setConnectionTimeouts(
    std::chrono::milliseconds dns,
    std::chrono::milliseconds tcp
)
{
    if (connectionState != ConnectionState::Idle)
    {
        throw std::logic_error("Impossible de modifier les timeouts apres connect()");
    }

    if (dns.count() <= 0 || tcp.count() <= 0)
    {
        throw std::invalid_argument("Les timeouts doivent etre positifs");
    }

    dnsTimeout = dns;
    tcpTimeout = tcp;
}

bool AsyncNetworkClient::isConnected() const
{
    return connected && !stopped;
}