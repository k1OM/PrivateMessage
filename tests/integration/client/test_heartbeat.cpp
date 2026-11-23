
#include "NetworkClient.hpp"
#include "HeartbeatManager.hpp"
#include "Protocol.hpp"

#include <chrono>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        NetworkClient client;

        client.connect(
            "127.0.0.1",
            Protocol::DEFAULT_PORT
        );

        std::cout
            << "Connexion etablie"
            << std::endl;

        Message hello(MessageType::Hello, {});

        client.sendMessage(hello);

        Message response = client.receiveMessage();

        if (response.getType() != MessageType::HelloAck)
        {
            throw std::runtime_error(
                "HELLO_ACK attendu"
            );
        }

        std::cout << "HELLO_ACK : OK" << std::endl;

        HeartbeatManager heartbeat(
            client,
            std::chrono::seconds(5)
        );

        heartbeat.run(4);

        client.disconnect();

        std::cout
            << "Test Heartbeat reussi"
            << std::endl;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "ERREUR : "
            << error.what()
            << std::endl;

        return 1;
    }

    return 0;
}
