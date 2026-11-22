
#include "AsyncHeartbeatManager.hpp"
#include "AsyncNetworkClient.hpp"
#include "Message.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

AsyncHeartbeatManager::AsyncHeartbeatManager(
    asio::io_context& context,
    std::shared_ptr<AsyncNetworkClient> client,
    std::chrono::milliseconds interval,
    std::chrono::milliseconds timeout
)
    : client(client),
      pingTimer(context),
      pongTimer(context),
      pingInterval(interval),
      pongTimeout(timeout),
      successfulPings(0),
      active(false),
      waitingForPong(false)
{
    if (!client)
    {
        throw std::invalid_argument("Client nul");
    }

    if (interval.count() <= 0 || timeout.count() <= 0)
    {
        throw std::invalid_argument("Delai invalide");
    }
}

void AsyncHeartbeatManager::setOnTimeout(std::function<void()> callback)
{
    onTimeoutCallback = std::move(callback);
}

void AsyncHeartbeatManager::start()
{
    std::cout
        << "[DEBUG] HeartbeatManager::start()"
        << std::endl;

    if (active)
    {
        std::cout
            << "[DEBUG] Heartbeat deja actif"
            << std::endl;

        return;
    }

    std::shared_ptr<AsyncNetworkClient> locked =
        client.lock();

    if (!locked || !locked->isConnected())
    {
        throw std::runtime_error(
            "Impossible de demarrer le heartbeat"
        );
    }

    active = true;
    waitingForPong = false;
    successfulPings = 0;

    schedulePing();

    std::cout
        << "[DEBUG] Heartbeat actif"
        << std::endl;

}

void AsyncHeartbeatManager::stop()
{
    if (!active)
    {
        return;
    }

    active = false;
    waitingForPong = false;

    asio::error_code error;

    pingTimer.cancel(error);
    pongTimer.cancel(error);
}

std::size_t AsyncHeartbeatManager::getSuccessfulPings() const
{
    return successfulPings;
}