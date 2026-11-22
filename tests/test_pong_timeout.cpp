
#include "AsyncNetworkClient.hpp"
#include "AsyncHeartbeatManager.hpp"
#include "Message.hpp"

#include <asio.hpp>

#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main()
{
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

    bool timeoutDetected = false;
    bool unexpectedDisconnect = false;
    bool watchdogExpired = false;

    // Garde-fou pour ne pas laisser le test tourner
    // indefiniment en cas de probleme.
    asio::steady_timer watchdog(context);

    watchdog.expires_after(std::chrono::seconds(12));

    watchdog.async_wait(
        [&watchdogExpired, weakClient, weakHeartbeat](
            const asio::error_code& error
        )
        {
            if (error)
            {
                return;
            }

            watchdogExpired = true;

            std::cerr
                << "ERREUR : delai global depasse"
                << std::endl;

            if (auto manager = weakHeartbeat.lock())
            {
                manager->stop();
            }

            if (auto connection = weakClient.lock())
            {
                connection->disconnect();
            }
        }
    );

    
    heartbeat->setOnTimeout(
        [&timeoutDetected, weakClient]()
        {
            timeoutDetected = true;

            std::cout
                << "Timeout PONG detecte : OK"
                << std::endl;

            // Le controleur n'est pas present dans ce test.
            // Nous simulons donc sa decision de deconnexion.

            std::shared_ptr<AsyncNetworkClient> connection =
                weakClient.lock();

            if (connection)
            {
                connection->disconnect();
            }
        }
    );


    client->setOnConnect(
        [weakClient]()
        {
            if (auto connection = weakClient.lock())
            {
                std::cout << "Connexion etablie" << std::endl;

                connection->sendMessage(
                    Message(MessageType::Hello, {})
                );
            }
        }
    );

    client->setOnMessage(
        [weakHeartbeat, weakClient](
            const Message& message
        )
        {
            if (message.getType() == MessageType::HelloAck)
            {
                std::cout << "HELLO_ACK recu" << std::endl;

                if (auto manager = weakHeartbeat.lock())
                {
                    manager->start();
                }
            }
            else
            {
                std::cerr
                    << "Message inattendu"
                    << std::endl;

                if (auto connection = weakClient.lock())
                {
                    connection->disconnect();
                }
            }
        }
    );

    client->setOnDisconnect(
        [&timeoutDetected, &unexpectedDisconnect, &watchdog](
            const std::string& reason
        )
        {
            std::cout
                << "Deconnexion : "
                << reason
                << std::endl;

            if (!timeoutDetected)
            {
                unexpectedDisconnect = true;
            }

            watchdog.cancel();
        }
    );

    try
    {
        client->connect("127.0.0.1", 5556);

        context.run();
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Exception : "
            << error.what()
            << std::endl;

        watchdogExpired = true;
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

    if (!timeoutDetected ||
        unexpectedDisconnect ||
        watchdogExpired)
    {
        std::cerr
            << "Test timeout PONG echoue"
            << std::endl;

        return 1;
    }

    std::cout
        << "Test timeout PONG : OK"
        << std::endl;

    return 0;
}
