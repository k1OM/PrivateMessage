
#include "AsyncNetworkClient.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

AsyncNetworkClient::AsyncNetworkClient(
    asio::io_context& context
)
    : socket(context),
      header{},
      payload{},
      outgoingMessages{},
      connected(false),
      stopped(false)
{
}

void AsyncNetworkClient::setOnConnect(
    ConnectCallback callback
)
{
    onConnect = std::move(callback);
}

void AsyncNetworkClient::setOnMessage(
    MessageCallback callback
)
{
    onMessage = std::move(callback);
}

void AsyncNetworkClient::setOnDisconnect(
    DisconnectCallback callback
)
{
    onDisconnect = std::move(callback);
}

bool AsyncNetworkClient::isConnected() const
{
    return connected && !stopped;
}