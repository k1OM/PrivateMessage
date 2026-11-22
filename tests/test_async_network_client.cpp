
#include "AsyncNetworkClient.hpp"
#include "Message.hpp"
#include "Protocol.hpp"

#include <asio.hpp>

#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main()
{
    asio::io_context context;

    std::shared_ptr<AsyncNetworkClient> client =
        std::make_shared<AsyncNetworkClient>(context);

    std::weak_ptr<AsyncNetworkClient> weakClient = client;

    bool success = false;
    bool failed = false;

    client->setOnConnect(
        [weakClient]()
        {
            std::shared_ptr<AsyncNetworkClient> locked =
                weakClient.lock();

            if (!locked)
            {
                return;
            }

            std::cout << "Connexion etablie" << std::endl;

            Message hello(MessageType::Hello, {});

            locked->sendMessage(hello);
        }
    );

    client->setOnMessage(
        [weakClient, &success, &failed](
            const Message& message
        )
        {
            std::shared_ptr<AsyncNetworkClient> locked =
                weakClient.lock();

            if (!locked)
            {
                return;
            }

            switch (message.getType())
            {
                case MessageType::HelloAck:
                {
                    std::cout << "HELLO_ACK recu" << std::endl;

                    Message ping(MessageType::Ping, {});

                    locked->sendMessage(ping);
                    break;
                }

                case MessageType::Pong:
                {
                    std::cout << "PONG recu" << std::endl;

                    success = true;
                    locked->disconnect();
                    break;
                }

                default:
                {
                    failed = true;

                    std::cerr
                        << "Message inattendu"
                        << std::endl;

                    locked->disconnect();
                    break;
                }
            }
        }
    );

    client->setOnDisconnect(
        [&success, &failed](const std::string& reason)
        {
            std::cout
                << "Deconnexion : "
                << reason
                << std::endl;

            if (!success)
            {
                failed = true;
            }
        }
    );

    try
    {
        client->connect(
            "127.0.0.1",
            Protocol::DEFAULT_PORT
        );

        context.run();
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Exception : "
            << error.what()
            << std::endl;

        return 1;
    }

    client.reset();

    if (!weakClient.expired())
    {
        std::cerr
            << "ERREUR : objet client encore vivant"
            << std::endl;

        return 1;
    }

    std::cout
        << "Memoire client : destruction confirmee"
        << std::endl;

    if (failed || !success)
    {
        return 1;
    }

    std::cout
        << "Test AsyncNetworkClient : OK"
        << std::endl;

    return 0;
}
