
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <vector>

enum class MessageType : std::uint8_t
{
    Hello = 1,
    HelloAck = 2,
    Ping = 3,
    Pong = 4,
    Chat = 5,
    Error = 6,
    PrivateChat = 7,
    SessionAssigned = 8
};

class Message
{
private:
    MessageType type;

    std::vector<std::uint8_t> payload;

public:
    Message(
        MessageType type,
        const std::vector<std::uint8_t>& payload
    );

    MessageType getType() const;

    const std::vector<std::uint8_t>& getPayload() const;
};

#endif
