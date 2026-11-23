
#ifndef MULTICLIENTTEST_HPP
#define MULTICLIENTTEST_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>
#include <mutex>
#include <unordered_set>

class MultiClientTest
{
private:
    std::mutex sessionMutex;
    std::string serverAddress;

    std::uint16_t serverPort;
    std::size_t clientCount;

    std::unordered_set<std::uint64_t> sessionIds;
    std::atomic<std::size_t> successfulClients;
    std::atomic<std::size_t> failedClients;

public:
    MultiClientTest(
        const std::string& address,
        std::uint16_t port,
        std::size_t count
    );

    bool run();

private:
    void runClient(std::size_t clientId);
};

#endif
