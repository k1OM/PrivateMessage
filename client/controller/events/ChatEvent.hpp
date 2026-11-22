
#ifndef CHATEVENT_HPP
#define CHATEVENT_HPP

#include <cstdint>
#include <string>

enum class ChatEventType
{
    Connected,
    Ready,
    Heartbeat,
    ChatMessage,
    HeartbeatTimeout,
    SessionAssigned,
    PrivateMessage,
    Disconnected,
    Error,
    ServerError
};

struct ChatEvent
{
    ChatEventType type;

    std::string text;

    std::uint64_t senderId = 0;
};

#endif
