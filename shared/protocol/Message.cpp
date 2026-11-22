
#include "Message.hpp"

Message::Message(
    MessageType type,
    const std::vector<std::uint8_t>& payload
)
    : type(type),
      payload(payload)
{
}

MessageType Message::getType() const
{
    return type;
}

const std::vector<std::uint8_t>& Message::getPayload() const
{
    return payload;
}
