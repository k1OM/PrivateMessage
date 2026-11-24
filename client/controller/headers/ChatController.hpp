
#ifndef CHATCONTROLLER_HPP
#define CHATCONTROLLER_HPP

#include "ChatEvent.hpp"

#include <asio.hpp>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

class AsyncNetworkClient;
class AsyncHeartbeatManager;
class Message;

class ChatController
{
private:
    asio::io_context context;

    asio::executor_work_guard<
        asio::io_context::executor_type
    > workGuard;

    std::thread networkThread;

    std::shared_ptr<AsyncNetworkClient> client;
    std::shared_ptr<AsyncHeartbeatManager> heartbeat;

    std::atomic<std::uint64_t> nextMessageId{1};

    std::mutex eventMutex;
    std::queue<ChatEvent> events;

    std::atomic<bool> started;
    std::atomic<bool> stopping;

public:
    ChatController();
    ~ChatController();

    ChatController(const ChatController&) = delete;
    ChatController& operator=(const ChatController&) = delete;

    void start(
        const std::string& address,
        std::uint16_t port
    );

    void sendPrivateMessage(std::uint64_t recipientId, const std::string& text);

    void stop();

    std::vector<ChatEvent> pollEvents();

private:
    void configureCallbacks();
    void handleConnected();
    void handleHeartbeatTimeout();

    void handleDisconnected(
        const std::string& reason
    );

    void pushEvent(
        ChatEventType type,
        const std::string& text
    );

    void handleIncomingMessage(
        const Message& message,
        std::weak_ptr<AsyncHeartbeatManager> weakHeartbeat
    );

    void pushEvent(
        ChatEventType type,
        const std::string& text,
        std::uint64_t senderId
    );

};

#endif
