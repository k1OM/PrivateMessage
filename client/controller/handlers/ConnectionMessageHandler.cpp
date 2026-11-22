
#include "ConnectionMessageHandler.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

ChatEvent ConnectionMessageHandler::handle(
    const Message& message
) const
{
    switch (message.getType())
    {
        case MessageType::HelloAck:
        {
            return ChatEvent{
                ChatEventType::Ready,
                "Serveur pret : HELLO_ACK"
            };
        }

        case MessageType::SessionAssigned:
        {
            const std::vector<std::uint8_t>& payload =
                message.getPayload();

            if (payload.size() != 8)
            {
                throw std::invalid_argument(
                    "SESSION_ASSIGNED invalide"
                );
            }

            std::uint64_t sessionId = 0;

            for (std::uint8_t byte : payload)
            {
                sessionId =
                    (sessionId << 8) |
                    static_cast<std::uint64_t>(byte);
            }

            if (sessionId == 0)
            {
                throw std::invalid_argument(
                    "Identifiant de session nul"
                );
            }

            return ChatEvent{
                ChatEventType::SessionAssigned,
                "Ma session : " +
                    std::to_string(sessionId)
            };
        }

        default:
        {
            throw std::invalid_argument(
                "Message de connexion attendu"
            );
        }
    }
}
