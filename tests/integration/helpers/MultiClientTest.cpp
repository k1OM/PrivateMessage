
#include "MultiClientTest.hpp"

#include "NetworkClient.hpp"
#include "Message.hpp"

#include <exception>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace
{
    std::mutex consoleMutex;
}

MultiClientTest::MultiClientTest(
    const std::string& address,
    std::uint16_t port,
    std::size_t count
)
    : serverAddress(address),
      serverPort(port),
      clientCount(count),
      successfulClients(0),
      failedClients(0)
{
}

void MultiClientTest::runClient(std::size_t clientId)
{
    try
    {
        NetworkClient client;

        client.connect(serverAddress, serverPort);

        // HELLO
        Message hello(MessageType::Hello, {});

        client.sendMessage(hello);

        Message helloResponse = client.receiveMessage();

        if (helloResponse.getType() != MessageType::HelloAck)
        {
            throw std::runtime_error("HELLO_ACK attendu");
        }

        // PING
        Message ping(MessageType::Ping, {});

        client.sendMessage(ping);

        Message pongResponse = client.receiveMessage();

        if (pongResponse.getType() != MessageType::Pong)
        {
            throw std::runtime_error("PONG attendu");
        }

        client.disconnect();

        successfulClients.fetch_add(1);

        std::lock_guard<std::mutex> lock(consoleMutex);

        std::cout
            << "[Client " << clientId << "] OK"
            << std::endl;
    }
    catch (const std::exception& error)
    {
        failedClients.fetch_add(1);

        std::lock_guard<std::mutex> lock(consoleMutex);

        std::cerr
            << "[Client " << clientId << "] ERREUR : "
            << error.what()
            << std::endl;
    }
}

bool MultiClientTest::run()
{
    std::vector<std::thread> threads;

    threads.reserve(clientCount);

    std::cout
        << "Demarrage de " << clientCount
        << " clients..."
        << std::endl;

    for (std::size_t i = 0; i < clientCount; ++i)
    {
        threads.emplace_back(
            &MultiClientTest::runClient,
            this,
            i + 1
        );
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    std::size_t success = successfulClients.load();
    std::size_t failure = failedClients.load();

    std::cout << "\n===== RESULTATS =====" << std::endl;

    std::cout << "Reussites : " << success << std::endl;
    std::cout << "Echecs    : " << failure << std::endl;

    return success == clientCount && failure == 0;
}
