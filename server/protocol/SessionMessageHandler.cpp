
#include "SessionMessageHandler.hpp"

#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

void SessionMessageHandler::setSessionId(std::uint64_t id)
{
    sessionId = id;
}

void SessionMessageHandler::setOnSend(SendCallback callback)
{
    onSend = std::move(callback);
}

void SessionMessageHandler::setOnClose(CloseCallback callback)
{
    onClose = std::move(callback);
}

void SessionMessageHandler::setOnChat(MessageCallback callback)
{
    onChat = std::move(callback);
}

void SessionMessageHandler::setOnPrivateChat(MessageCallback callback)
{
    onPrivateChat = std::move(callback);
}

SessionProtocolState SessionMessageHandler::getState() const
{
    return state;
}

void SessionMessageHandler::processMessage(const Message& message)
{
    // 1. Une session en fermeture ignore les messages.

    if (state == SessionProtocolState::Closing)
    {
        return;
    }

    // 2. Avant HELLO, seul HELLO est accepte.

    if (state == SessionProtocolState::AwaitingHello)
    {
        if (message.getType() != MessageType::Hello)
        {
            std::cerr
                << "[SESSION " << sessionId << "] "
                << "Message interdit avant HELLO"
                << std::endl;

            state = SessionProtocolState::Closing;

            if (onClose)
            {
                onClose();
            }

            return;
        }
    }

    // 3. Une session active ne peut pas
    // recommencer son initialisation.

    if (state == SessionProtocolState::Active)
    {
        if (message.getType() == MessageType::Hello)
        {
            std::cerr
                << "[SESSION " << sessionId << "] "
                << "HELLO deja recu"
                << std::endl;

            state = SessionProtocolState::Closing;

            if (onClose)
            {
                onClose();
            }

            return;
        }
    }

    // 4. Traitement normal du protocole.
    switch (message.getType())
    {
        
        case MessageType::Hello:
        {
            //std::cout << "HELLO recu" << std::endl;

            if (sessionId == 0)
            {
                state = SessionProtocolState::Closing;

                if (onClose)
                {
                    onClose();
                }

                return;
            }

            // Transition vers une session active.

            state = SessionProtocolState::Active;

            // Envoyer HELLO_ACK.

            Message response(
                MessageType::HelloAck,
                {}
            );

            if (onSend)
            {
                onSend(response);
            }

            std::vector<std::uint8_t> idBytes;

            idBytes.reserve(8);

            for (int i = 7; i >= 0; --i)
            {
                std::uint8_t byte =
                    static_cast<std::uint8_t>(
                        (sessionId >> (i * 8)) & 0xFF
                    );

                idBytes.push_back(byte);
            }

            Message assignment(
                MessageType::SessionAssigned,
                idBytes
            );

            if (onSend)
            {
                onSend(assignment);
            }

            break;
        }

        case MessageType::Ping:
        {
            //std::cout << "[SESSION " << sessionId << "] " << "PING recu - timeout reinitialise" << std::endl;

            Message response(
                MessageType::Pong,
                {}
            );

            if (onSend)
            {
                onSend(response);
            }

            break;
        }

        case MessageType::Chat:
        {
            //std::cout << "CHAT recu" << std::endl;

            if (onChat)
            {
                onChat(message);
            }

            break;
        }

        case MessageType::PrivateChat:
        {
            //std::cout << "PRIVATE_CHAT recu" << std::endl;

            if (onPrivateChat)
            {
                onPrivateChat(message);
            }

            break;
        }

        default:
        {
            std::cout << "Message non gere" << std::endl;
            break;
        }
    }
}
