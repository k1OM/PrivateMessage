#include "ClientSession.hpp"
#include "Message.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

void ClientSession::processMessage(const Message& message)
{
    switch (message.getType())
    {
        case MessageType::Hello:
        {
            std::cout << "HELLO recu" << std::endl;

            // 1. Verifier l'identifiant de session

            if (sessionId == 0)
            {
                closeConnection();
                return;
            }

            // 2. Envoyer HELLO_ACK

            Message response(
                MessageType::HelloAck,
                {}
            );

            sendMessage(response);

            // 3. Encoder l'identifiant sur 8 octets
            // en big-endian

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

            // 4. Envoyer SESSION_ASSIGNED

            Message assignment(
                MessageType::SessionAssigned,
                idBytes
            );

            sendMessage(assignment);

            break;
        }

        case MessageType::PrivateChat:
        {
            std::cout
                << "PRIVATE_CHAT recu"
                << std::endl;

            if (onPrivateChat)
            {
                onPrivateChat(message);
            }

            break;
        }

        case MessageType::Chat:
        {
            std::cout
                << "CHAT recu"
                << std::endl;

            if (onChat)
            {
                onChat(message);
            }

            break;
        }

        case MessageType::Ping:
        {
            //std::cout << "PING recu" << std::endl;

            std::cout
            << "[SESSION " << sessionId << "] "
            << "PING recu - timeout reinitialise"
            << std::endl;

            Message response(
                MessageType::Pong,
                {}
            );

            sendMessage(response);

            break;
        }

        default:
        {
            std::cout
                << "Message non gere"
                << std::endl;

            break;
        }
    }
}