
#ifndef HEARTBEATMESSAGEHANDLER_HPP
#define HEARTBEATMESSAGEHANDLER_HPP

#include "ChatEvent.hpp"
#include "Message.hpp"

class HeartbeatMessageHandler
{
public:
    HeartbeatMessageHandler() = default;

    ChatEvent handle(
        const Message& message
    ) const;
};

#endif
