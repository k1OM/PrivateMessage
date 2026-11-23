
#include "NetworkClient.hpp"
#include "Message.hpp"
#include "Protocol.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

// Recevoir et decoder un identifiant de session.
std::uint64_t connectAndHandshake(
    NetworkClient& client,
    std::uint16_t port
)
{
    client.connect("127.0.0.1", port);

    client.sendMessage(Message(MessageType::Hello, {}));

    Message helloAck = client.receiveMessage();

    if (helloAck.getType() != MessageType::HelloAck)
    {
        throw std::runtime_error("HELLO_ACK attendu");
    }

    Message assignment = client.receiveMessage();

    if (assignment.getType() != MessageType::SessionAssigned)
    {
        throw std::runtime_error("SESSION_ASSIGNED attendu");
    }

    const std::vector<std::uint8_t>& payload =
        assignment.getPayload();

    if (payload.size() != 8)
    {
        throw std::runtime_error("Session ID invalide : taille");
    }

    std::uint64_t sessionId = 0;

    for (std::uint8_t byte : payload)
    {
        sessionId =
            (sessionId << 8) |
            static_cast<std::uint64_t>(byte);
    }

    if (sessionId == 0)
    {
        throw std::runtime_error("Session ID nul");
    }

    return sessionId;
}

// Verifier qu'un client encore connecte repond au PING.
void verifyHeartbeat(NetworkClient& client)
{
    client.sendMessage(Message(MessageType::Ping, {}));

    Message response = client.receiveMessage();

    if (response.getType() != MessageType::Pong)
    {
        throw std::runtime_error("PONG attendu");
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::uint16_t port = Protocol::DEFAULT_PORT;

        if (argc > 2)
        {
            throw std::runtime_error(
                "Usage : test_session_disconnect [port]"
            );
        }

        if (argc == 2)
        {
            std::size_t parsedCharacters = 0;

            unsigned long parsedPort =
                std::stoul(argv[1], &parsedCharacters);

            if (parsedCharacters != std::string(argv[1]).size()
                || parsedPort == 0
                || parsedPort > 65535)
            {
                throw std::runtime_error("Port invalide");
            }

            port = static_cast<std::uint16_t>(parsedPort);
        }

        constexpr std::size_t CLIENT_COUNT = 5;
        constexpr std::size_t DISCONNECTED_INDEX = 2;

        std::vector<std::unique_ptr<NetworkClient>> clients;
        std::unordered_set<std::uint64_t> sessionIds;

        clients.reserve(CLIENT_COUNT);

        std::cout << "Connexion de 5 clients..." << std::endl;

        // 1. Connecter les cinq clients.
        for (std::size_t i = 0; i < CLIENT_COUNT; ++i)
        {
            std::unique_ptr<NetworkClient> client =
                std::make_unique<NetworkClient>();

            std::uint64_t id =
                connectAndHandshake(*client, port);

            if (!sessionIds.insert(id).second)
            {
                throw std::runtime_error("Session ID duplique");
            }

            std::cout
                << "Client " << (i + 1)
                << " : Session " << id
                << std::endl;

            clients.push_back(std::move(client));
        }

        // 2. Verifier les cinq clients.
        for (const auto& client : clients)
        {
            verifyHeartbeat(*client);
        }

        std::cout << "5 heartbeats reussis" << std::endl;

        // 3. Deconnecter volontairement le client 3.
        clients[DISCONNECTED_INDEX]->disconnect();

        std::cout << "Client 3 deconnecte" << std::endl;

        // 4. Verifier que les quatre autres fonctionnent.
        std::size_t survivors = 0;

        for (std::size_t i = 0; i < clients.size(); ++i)
        {
            if (i == DISCONNECTED_INDEX)
            {
                continue;
            }

            verifyHeartbeat(*clients[i]);

            ++survivors;
        }

        if (survivors != CLIENT_COUNT - 1)
        {
            throw std::runtime_error(
                "Nombre de survivants incorrect"
            );
        }

        std::cout
            << "4 clients toujours fonctionnels"
            << std::endl;

        // 5. Accepter un nouveau client.
        NetworkClient newClient;

        std::uint64_t newSessionId =
            connectAndHandshake(newClient, port);

        if (!sessionIds.insert(newSessionId).second)
        {
            throw std::runtime_error(
                "Nouvelle session deja utilisee"
            );
        }

        verifyHeartbeat(newClient);

        std::cout
            << "Nouveau client : Session "
            << newSessionId
            << std::endl;

        // 6. Deconnexion propre.
        newClient.disconnect();

        for (const auto& client : clients)
        {
            client->disconnect();
        }

        std::cout
            << "\nSessionDisconnectIntegration : OK"
            << std::endl;

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Test echoue : "
            << error.what()
            << std::endl;

        return 1;
    }
}
