
#include "HeartbeatMessageHandler.hpp"

#include <stdexcept>

ChatEvent HeartbeatMessageHandler::handle(
    const Message& message
) const
{
    if (message.getType() != MessageType::Pong)
    {
        throw std::invalid_argument(
            "PONG attendu"
        );
    }

    return ChatEvent{
        ChatEventType::Heartbeat,
        "PONG recu"
    };
}
