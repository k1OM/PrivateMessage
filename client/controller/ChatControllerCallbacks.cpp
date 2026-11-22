#include "ChatController.hpp"
#include "AsyncNetworkClient.hpp"
#include "AsyncHeartbeatManager.hpp"
#include "Message.hpp"

#include <exception>
#include <string>

void ChatController::configureCallbacks()
{
    std::weak_ptr<AsyncHeartbeatManager> weakHeartbeat =
        heartbeat;

    client->setOnConnect(
        [this]()
        {
            handleConnected();
        }
    );

    client->setOnMessage(
        [this, weakHeartbeat](const Message& message)
        {
            handleIncomingMessage(
                message,
                weakHeartbeat
            );
        }
    );

    client->setOnDisconnect(
        [this](const std::string& reason)
        {
            handleDisconnected(reason);
        }
    );

    heartbeat->setOnTimeout(
        [this]()
        {
            handleHeartbeatTimeout();
        }
    );
}

void ChatController::handleConnected()
{
    pushEvent(
        ChatEventType::Connected,
        "Connexion TCP etablie"
    );

    if (!client)
    {
        return;
    }

    try
    {
        client->sendMessage(
            Message(MessageType::Hello, {})
        );
    }
    catch (const std::exception& error)
    {
        pushEvent(
            ChatEventType::Error,
            error.what()
        );

        client->disconnect();
    }
}

void ChatController::handleHeartbeatTimeout()
{
    pushEvent(
        ChatEventType::HeartbeatTimeout,
        "Le serveur ne repond plus aux PING"
    );

    if (client)
    {
        client->disconnect();
    }
}

void ChatController::handleDisconnected(
    const std::string& reason
)
{
    if (heartbeat)
    {
        heartbeat->stop();
    }

    pushEvent(
        ChatEventType::Disconnected,
        reason
    );
}