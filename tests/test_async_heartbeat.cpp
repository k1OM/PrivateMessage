
#include "AsyncNetworkClient.hpp"
#include "AsyncHeartbeatManager.hpp"
#include "Message.hpp"
#include "Protocol.hpp"

#include <asio.hpp>
#include <cstdint>
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

int main(int argc, char* argv[])
{
    constexpr std::size_t TARGET_PINGS = 4;

    asio::io_context context;

    std::shared_ptr<AsyncNetworkClient> client =
        std::make_shared<AsyncNetworkClient>(context);

    std::shared_ptr<AsyncHeartbeatManager> heartbeat =
        std::make_shared<AsyncHeartbeatManager>(
            context,
            client,
            std::chrono::seconds(5),
            std::chrono::seconds(3)
        );

    std::weak_ptr<AsyncNetworkClient> weakClient = client;
    std::weak_ptr<AsyncHeartbeatManager> weakHeartbeat =
        heartbeat;

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

            std::cout
                << "Connexion etablie"
                << std::endl;

            locked->sendMessage(
                Message(MessageType::Hello, {})
            );
        }
    );

    client->setOnMessage(
        [weakClient, weakHeartbeat, &success, &failed](
            const Message& message
        )
        {
            std::shared_ptr<AsyncNetworkClient> lockedClient =
                weakClient.lock();

            std::shared_ptr<AsyncHeartbeatManager> lockedHeartbeat =
                weakHeartbeat.lock();

            if (!lockedClient || !lockedHeartbeat)
            {
                return;
            }

            switch (message.getType())
            {
                case MessageType::HelloAck:
                {
                    std::cout
                        << "HELLO_ACK recu"
                        << std::endl;

                    lockedHeartbeat->start();
                    break;
                }

                
                case MessageType::SessionAssigned:
                {
                    std::cout
                        << "Identifiant de session recu"
                        << std::endl;

                    break;
                }


                case MessageType::Pong:
                {
                    lockedHeartbeat->onPong();

                    if (lockedHeartbeat->getSuccessfulPings()
                        >= TARGET_PINGS)
                    {
                        success = true;

                        lockedHeartbeat->stop();
                        lockedClient->disconnect();
                    }

                    break;
                }

                default:
                {
                    failed = true;

                    std::cerr
                        << "Message inattendu"
                        << std::endl;

                    lockedHeartbeat->stop();
                    lockedClient->disconnect();

                    break;
                }
            }
        }
    );

    client->setOnDisconnect(
        [weakHeartbeat, &success, &failed](
            const std::string& reason
        )
        {
            if (auto locked = weakHeartbeat.lock())
            {
                locked->stop();
            }

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

    std::uint16_t port = Protocol::DEFAULT_PORT;

    try
    {
        if (argc > 2)
        {
            throw std::invalid_argument(
                "Usage : test_async_heartbeat [port]"
            );
        }

        if (argc == 2)
        {
            std::string argument = argv[1];
            std::size_t position = 0;

            unsigned long value = std::stoul(
                argument,
                &position
            );

            if (position != argument.size() ||
                value == 0 ||
                value > 65535)
            {
                throw std::invalid_argument(
                    "Port invalide"
                );
            }

            port = static_cast<std::uint16_t>(value);
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << "Erreur : " << error.what() << std::endl;
        return 1;
    }

    try
    {
        client->connect(
            "127.0.0.1",
            port
        );

        context.run();
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Exception reseau : "
            << error.what()
            << std::endl;

        failed = true;

        heartbeat->stop();
        client->disconnect();

        context.restart();
        context.poll();
    }

    heartbeat->stop();
    client->disconnect();

    heartbeat.reset();
    client.reset();

    if (!weakHeartbeat.expired() || !weakClient.expired())
    {
        std::cerr
            << "ERREUR : objets encore vivants"
            << std::endl;

        return 1;
    }

    if (failed || !success)
    {
        std::cerr
            << "Test heartbeat asynchrone echoue"
            << std::endl;

        return 1;
    }

    std::cout
        << "Destruction des objets confirmee"
        << std::endl;

    std::cout
        << "Test AsyncHeartbeatManager : OK"
        << std::endl;

    return 0;
}
