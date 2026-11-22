#include "AsyncHeartbeatManager.hpp"
#include "AsyncNetworkClient.hpp"
#include "Message.hpp"

#include <exception>
#include <iostream>
#include <memory>

void AsyncHeartbeatManager::sendPing()
{
    if (!active || waitingForPong)
    {
        return;
    }

    std::shared_ptr<AsyncNetworkClient> locked = client.lock();

    if (!locked || !locked->isConnected())
    {
        stop();
        return;
    }

    Message ping(MessageType::Ping, {});

    try
    {
        locked->sendMessage(ping);
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Erreur envoi PING : "
            << error.what()
            << std::endl;

        stop();
        locked->disconnect();
        return;
    }

    waitingForPong = true;

    std::cout << "Heartbeat : PING envoye" << std::endl;

    startPongTimeout();
}

void AsyncHeartbeatManager::onPong()
{
    if (!active || !waitingForPong)
    {
        return;
    }

    waitingForPong = false;

    asio::error_code error;
    pongTimer.cancel(error);

    ++successfulPings;

    std::cout << "Heartbeat : PONG recu (" << successfulPings << ")" << std::endl;

    schedulePing();
}