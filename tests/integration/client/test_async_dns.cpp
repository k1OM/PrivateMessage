
#include "AsyncNetworkClient.hpp"

#include <asio.hpp>

#include <chrono>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main()
{
    try
    {
        asio::io_context context;

        asio::ip::tcp::acceptor acceptor(
            context,
            asio::ip::tcp::endpoint(
                asio::ip::address_v4::loopback(),
                0
            )
        );

        const std::uint16_t port = acceptor.local_endpoint().port();

        bool connectionEstablished = false;
        bool timedOut = false;

        std::shared_ptr<AsyncNetworkClient> client =
            std::make_shared<AsyncNetworkClient>(context);

        asio::steady_timer timeout(context);

        acceptor.async_accept(
            [](const asio::error_code& error,
               asio::ip::tcp::socket socket)
            {
                if (!error)
                {
                    std::cout
                        << "[SERVER] Client accepte"
                        << std::endl;
                }
            }
        );

        client->setOnConnect(
            [&]()
            {
                connectionEstablished = true;

                std::cout
                    << "[CLIENT] Connexion TCP reussie"
                    << std::endl;

                timeout.cancel();
                client->disconnect();
            }
        );

        client->setOnDisconnect(
            [](const std::string& reason)
            {
                std::cout
                    << "[CLIENT] Deconnexion : "
                    << reason
                    << std::endl;
            }
        );

        timeout.expires_after(
            std::chrono::seconds(5)
        );

        timeout.async_wait(
            [&](const asio::error_code& error)
            {
                if (error)
                {
                    return;
                }

                timedOut = true;

                std::cerr
                    << "[TEST] Timeout DNS/TCP"
                    << std::endl;

                client->disconnect();

                asio::error_code cancelError;
                acceptor.cancel(cancelError);
            }
        );

        std::cout
            << "[TEST] Resolution de localhost:"
            << port
            << std::endl;

        client->connect("localhost", port);


        bool secondConnectionRejected = false;

        try
        {
            client->connect("localhost", port);
        }
        catch (const std::runtime_error& error)
        {
            secondConnectionRejected = true;

            std::cout
                << "[TEST] Deuxieme connexion refusee : "
                << error.what()
                << std::endl;
        }

        if (!secondConnectionRejected)
        {
            std::cerr
                << "[ECHEC] Double connexion acceptee"
                << std::endl;

            client->disconnect();

            return 1;
        }

        context.run();

        if (timedOut)
        {
            std::cerr
                << "[ECHEC] Delai depasse"
                << std::endl;

            return 1;
        }

        if (!connectionEstablished)
        {
            std::cerr
                << "[ECHEC] Connexion non etablie"
                << std::endl;

            return 1;
        }

        std::cout
            << "[OK] Resolution DNS et connexion TCP"
            << std::endl;

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "[ECHEC] "
            << error.what()
            << std::endl;

        return 1;
    }
}
