
#ifndef CONNECTIONMESSAGEHANDLER_HPP
#define CONNECTIONMESSAGEHANDLER_HPP

#include "ChatEvent.hpp"
#include "Message.hpp"

class ConnectionMessageHandler
{
public:
    ConnectionMessageHandler() = default;

    ChatEvent handle(
        const Message& message
    ) const;
};

#endif
