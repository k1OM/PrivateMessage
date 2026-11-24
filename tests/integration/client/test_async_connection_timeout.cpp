
#include "AsyncNetworkClient.hpp"

#include <asio.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

class AsyncNetworkClientTestAccess
{
public:
    static void startTimeout(
        const std::shared_ptr<AsyncNetworkClient>& client,
        std::chrono::milliseconds duration,
        bool dns
    )
    {
        if (dns)
        {
            client->connectionState =
                AsyncNetworkClient::ConnectionState::Resolving;

            client->startConnectionTimeout(
                AsyncNetworkClient::ConnectionState::Resolving,
                duration,
                "Timeout : resolution DNS"
            );
        }
        else
        {
            client->connectionState =
                AsyncNetworkClient::ConnectionState::Connecting;

            client->startConnectionTimeout(
                AsyncNetworkClient::ConnectionState::Connecting,
                duration,
                "Timeout : connexion TCP"
            );
        }
    }
};

bool runTimeoutTest(
    bool dns,
    const std::string& expectedReason
)
{
    asio::io_context context;

    std::shared_ptr<AsyncNetworkClient> client =
        std::make_shared<AsyncNetworkClient>(context);

    int disconnectCount = 0;
    std::string receivedReason;

    client->setOnDisconnect(
        [&](const std::string& reason)
        {
            ++disconnectCount;
            receivedReason = reason;
        }
    );

    AsyncNetworkClientTestAccess::startTimeout(
        client,
        std::chrono::milliseconds(30),
        dns
    );

    context.run();

    if (disconnectCount != 1)
    {
        std::cerr
            << "[ECHEC] Nombre de deconnexions incorrect"
            << std::endl;

        return false;
    }

    if (receivedReason != expectedReason)
    {
        std::cerr
            << "[ECHEC] Motif incorrect : "
            << receivedReason
            << std::endl;

        return false;
    }

    if (client->isConnected())
    {
        std::cerr
            << "[ECHEC] Client encore connecte"
            << std::endl;

        return false;
    }

    std::cout
        << "[OK] "
        << expectedReason
        << std::endl;

    return true;
}

int main()
{
    if (!runTimeoutTest(
        true,
        "Timeout : resolution DNS"))
    {
        return 1;
    }

    if (!runTimeoutTest(
        false,
        "Timeout : connexion TCP"))
    {
        return 1;
    }

    std::cout
        << "[OK] Tests des timeouts termines"
        << std::endl;

    return 0;
}
