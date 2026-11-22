#ifndef PRIVATEMESSAGEHANDLER_HPP
#define PRIVATEMESSAGEHANDLER_HPP

#include "ChatEvent.hpp"
#include "Message.hpp"

class PrivateMessageHandler
{
public:
    PrivateMessageHandler() = default;

    ChatEvent handle(
        const Message& message
    ) const;
};

#endif
