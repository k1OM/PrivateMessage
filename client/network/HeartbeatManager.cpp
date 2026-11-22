
#include "HeartbeatManager.hpp"

#include "NetworkClient.hpp"
#include "Message.hpp"

#include <stdexcept>
#include <thread>
#include <iostream>

HeartbeatManager::HeartbeatManager(
    NetworkClient& client,
    std::chrono::milliseconds interval
)
    : client(client),
      interval(interval)
{
    if (interval.count() <= 0)
    {
        throw std::invalid_argument(
            "Intervalle heartbeat invalide"
        );
    }
}

void HeartbeatManager::sendHeartbeat()
{
    if (!client.isConnected())
    {
        throw std::runtime_error(
            "Client non connecte"
        );
    }

    Message ping(MessageType::Ping, {});

    client.sendMessage(ping);

    Message response = client.receiveMessage();

    if (response.getType() != MessageType::Pong)
    {
        throw std::runtime_error(
            "Reponse PONG attendue"
        );
    }

    std::cout << "PING -> PONG : OK" << std::endl;
}

void HeartbeatManager::run(std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        sendHeartbeat();

        if (i + 1 < count)
        {
            asio::steady_timer
        }
    }
}
