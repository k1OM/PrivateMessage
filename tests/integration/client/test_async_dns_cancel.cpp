
#include "AsyncNetworkClient.hpp"

#include <asio.hpp>

#include <iostream>
#include <memory>
#include <string>

int main()
{
    asio::io_context context;

    std::shared_ptr<AsyncNetworkClient> client =
        std::make_shared<AsyncNetworkClient>(context);

    bool connected = false;
    int disconnectCount = 0;

    client->setOnConnect(
        [&connected]()
        {
            connected = true;
        }
    );

    client->setOnDisconnect(
        [&disconnectCount](const std::string& reason)
        {
            ++disconnectCount;

            std::cout
                << "[TEST] Deconnexion : "
                << reason
                << std::endl;
        }
    );

    // Demarrer une resolution asynchrone.
    client->connect("localhost", 5555);

    // Demander l'arret avant de traiter les callbacks.
    client->disconnect();

    // Executer les callbacks de terminaison / annulation.
    context.run();

    if (connected)
    {
        std::cerr
            << "[ECHEC] Connexion apres deconnexion"
            << std::endl;

        return 1;
    }

    if (client->isConnected())
    {
        std::cerr
            << "[ECHEC] Client encore connecte"
            << std::endl;

        return 1;
    }

    if (disconnectCount != 1)
    {
        std::cerr
            << "[ECHEC] Nombre de deconnexions : "
            << disconnectCount
            << std::endl;

        return 1;
    }

    std::cout
        << "[OK] Annulation DNS et etat du client"
        << std::endl;

    return 0;
}
