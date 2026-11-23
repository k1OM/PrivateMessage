
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

        // 1. Envoyer HELLO.
        Message hello(MessageType::Hello, {});

        client.sendMessage(hello);

        // 2. Verifier HELLO_ACK.
        Message helloResponse = client.receiveMessage();

        if (helloResponse.getType() != MessageType::HelloAck)
        {
            throw std::runtime_error("HELLO_ACK attendu");
        }

        // 3. Recevoir SESSION_ASSIGNED.
        Message sessionResponse = client.receiveMessage();

        if (sessionResponse.getType() != MessageType::SessionAssigned)
        {
            throw std::runtime_error("SESSION_ASSIGNED attendu");
        }

        // 4. Verifier la taille du payload.
        const std::vector<std::uint8_t>& payload =
            sessionResponse.getPayload();

        if (payload.size() != sizeof(std::uint64_t))
        {
            throw std::runtime_error(
                "Taille de SESSION_ASSIGNED invalide"
            );
        }

        // 5. Decoder le Session ID en Big-Endian.
        std::uint64_t receivedSessionId = 0;

        for (std::uint8_t byte : payload)
        {
            receivedSessionId =
                (receivedSessionId << 8) |
                static_cast<std::uint64_t>(byte);
        }

        if (receivedSessionId == 0)
        {
            throw std::runtime_error("Session ID invalide");
        }

        // 6. Verifier que le Session ID est unique.
        {
            std::lock_guard<std::mutex> lock(sessionMutex);

            bool inserted =
                sessionIds.insert(receivedSessionId).second;

            if (!inserted)
            {
                throw std::runtime_error(
                    "Session ID duplique"
                );
            }
        }

        // 7. Envoyer PING.
        Message ping(MessageType::Ping, {});

        client.sendMessage(ping);

        // 8. Verifier PONG.
        Message pongResponse = client.receiveMessage();

        if (pongResponse.getType() != MessageType::Pong)
        {
            throw std::runtime_error("PONG attendu");
        }

        // 9. Deconnexion propre.
        client.disconnect();

        successfulClients.fetch_add(1);

        // 10. Afficher le resultat.
        {
            std::lock_guard<std::mutex> lock(consoleMutex);

            std::cout
                << "[Client " << clientId << "] "
                << "Session ID : " << receivedSessionId
                << " - OK"
                << std::endl;
        }
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

    // 1. Demarrer tous les clients.
    for (std::size_t i = 0; i < clientCount; ++i)
    {
        threads.emplace_back(
            &MultiClientTest::runClient,
            this,
            i + 1
        );
    }

    // 2. Attendre la fin de tous les clients.
    for (std::thread& thread : threads)
    {
        thread.join();
    }

    // 3. Recuperer les resultats.
    std::size_t success = successfulClients.load();
    std::size_t failure = failedClients.load();

    std::size_t uniqueSessions = 0;

    {
        std::lock_guard<std::mutex> lock(sessionMutex);

        uniqueSessions = sessionIds.size();
    }

    // 4. Afficher les resultats.
    std::cout << "\n===== RESULTATS =====" << std::endl;

    std::cout << "Reussites          : " << success << std::endl;
    std::cout << "Echecs             : " << failure << std::endl;
    std::cout << "Sessions distinctes: " << uniqueSessions << std::endl;

    // 5. Valider le test.
    return success == clientCount
        && failure == 0
        && uniqueSessions == clientCount;
}