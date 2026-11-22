
#ifndef ASYNCHEARTBEATMANAGER_HPP
#define ASYNCHEARTBEATMANAGER_HPP

#include <asio.hpp>

#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>

class AsyncNetworkClient;

class AsyncHeartbeatManager :
    public std::enable_shared_from_this<AsyncHeartbeatManager>
{
private:
    std::weak_ptr<AsyncNetworkClient> client;

    asio::steady_timer pingTimer;
    asio::steady_timer pongTimer;

    std::chrono::milliseconds pingInterval;
    std::chrono::milliseconds pongTimeout;

    std::size_t successfulPings;

    bool active;
    bool waitingForPong;

    // Callback appele lorsqu'un PONG expire
    std::function<void()> onTimeoutCallback;

public:
    AsyncHeartbeatManager(
        asio::io_context& context,
        std::shared_ptr<AsyncNetworkClient> client,
        std::chrono::milliseconds interval,
        std::chrono::milliseconds timeout
    );

    void start();
    void stop();

    void onPong();

    void setOnTimeout(std::function<void()> callback);

    std::size_t getSuccessfulPings() const;

private:
    void schedulePing();
    void sendPing();
    void startPongTimeout();
};

#endif
