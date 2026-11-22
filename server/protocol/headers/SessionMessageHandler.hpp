
#ifndef SESSIONMESSAGEHANDLER_HPP
#define SESSIONMESSAGEHANDLER_HPP

#include <cstdint>
#include <functional>

#include "Message.hpp"

enum class SessionProtocolState
{
    AwaitingHello,
    Active,
    Closing
};

class SessionMessageHandler
{
public:
    using SendCallback = std::function<void(const Message&)>;

    using CloseCallback = std::function<void()>;

    using MessageCallback = std::function<void(const Message&)>;

private:
    std::uint64_t sessionId = 0;

    SendCallback onSend;
    CloseCallback onClose;

    MessageCallback onChat;
    MessageCallback onPrivateChat;

    SessionProtocolState state = SessionProtocolState::AwaitingHello;

public:
    SessionMessageHandler() = default;

    void setSessionId(std::uint64_t id);

    void setOnSend(SendCallback callback);
    void setOnClose(CloseCallback callback);

    void setOnChat(MessageCallback callback);
    void setOnPrivateChat(MessageCallback callback);

    void processMessage(const Message& message);

    SessionProtocolState getState() const;
};

#endif
