#ifndef SESSIONINACTIVITYMONITOR_HPP
#define SESSIONINACTIVITYMONITOR_HPP

#include <asio.hpp>

#include <chrono>
#include <functional>
#include <memory>
#include <cstdint>

class SessionInactivityMonitor :
    public std::enable_shared_from_this<SessionInactivityMonitor>
{
private:
    std::uint64_t generation = 0;
    
    asio::steady_timer timer;

    std::chrono::milliseconds timeout;

    std::function<void()> onTimeout;

    bool active = false;

public:
    SessionInactivityMonitor(
        asio::any_io_executor executor,
        std::chrono::milliseconds timeout
    );

    void setOnTimeout(std::function<void()> callback);

    void start();
    void reset();
    void stop();

private:
    void scheduleTimeout();
};

#endif
