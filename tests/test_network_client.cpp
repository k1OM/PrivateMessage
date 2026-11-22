
#include "NetworkClient.hpp"

#include <exception>
#include <iostream>
#include <vector>

int main()
{
    try
    {
        NetworkClient client;

        client.connect("127.0.0.1", 5555);

        std::cout << "Connexion reussie" << std::endl;

        // Test HELLO
        Message hello(MessageType::Hello, {});

        client.sendMessage(hello);

        Message response = client.receiveMessage();

        if (response.getType() != MessageType::HelloAck)
        {
            std::cerr << "HELLO_ACK attendu" << std::endl;
            return 1;
        }

        std::cout << "HELLO_ACK recu !" << std::endl;

        // Test PING
        Message ping(MessageType::Ping, {});

        client.sendMessage(ping);

        Message pong = client.receiveMessage();

        if (pong.getType() != MessageType::Pong)
        {
            std::cerr << "PONG attendu" << std::endl;
            return 1;
        }

        std::cout << "PONG recu !" << std::endl;

        client.disconnect();

        std::cout << "Deconnexion propre" << std::endl;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Erreur : "
            << error.what()
            << std::endl;

        return 1;
    }

    return 0;
}
