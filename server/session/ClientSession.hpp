#ifndef CLIENTSESSION_HPP
#define CLIENTSESSION_HPP

#include <asio.hpp>

#include <array>
#include <cstdint>
#include <deque>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>

#include "Message.hpp"
#include "MessageCodec.hpp"
#include "SessionInactivityMonitor.hpp"
#include "SessionMessageHandler.hpp"

class ClientSession :
    public std::enable_shared_from_this<ClientSession>
{
private:
    asio::ip::tcp::socket socket;
    std::shared_ptr<SessionInactivityMonitor> inactivityMonitor;

    MessageCodec::Header header;

    std::uint64_t sessionId = 0;

    SessionMessageHandler messageHandler;

    std::function<void(const Message&)> onChat;
    std::function<void(const Message&)> onPrivateChat;

    std::vector<std::uint8_t> payload;
    std::function<void()> onClose;
    std::deque<std::vector<std::uint8_t>> outgoingMessages;

    static constexpr std::uint32_t MAX_PAYLOAD = 65536;
    static constexpr std::uint32_t TIMEOUT_SECONDS = 15;

    bool closed = false;

public:
    explicit ClientSession(asio::ip::tcp::socket socket);
    void setOnClose(std::function<void()> callback);
    void setOnChat(std::function<void(const Message&)> callback);
    void sendMessage(const Message& message);
    void setSessionId(std::uint64_t id);
    void setOnPrivateChat(std::function<void(const Message&)> callback);
    void start();
    void stop();

private:
    void readHeader();
    void readPayload(std::uint32_t length);    
    void writeNext();
    void closeConnection();
};

#endif
