
#ifndef MULTICLIENTTEST_HPP
#define MULTICLIENTTEST_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>

class MultiClientTest
{
private:
    std::string serverAddress;
    std::uint16_t serverPort;

    std::size_t clientCount;

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
