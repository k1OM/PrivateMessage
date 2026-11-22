
#ifndef HEARTBEATMANAGER_HPP
#define HEARTBEATMANAGER_HPP

#include <chrono>
#include <cstddef>
#include <functional>

class NetworkClient;

class HeartbeatManager
{
private:
    NetworkClient& client;

    std::chrono::milliseconds interval;

public:
    HeartbeatManager(
        NetworkClient& client,
        std::chrono::milliseconds interval
    );

    void setOnTimeout(std::function<void()> callback);

    void run(std::size_t count);

private:
    void sendHeartbeat();
};

#endif
