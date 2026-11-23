
#include <asio.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include "Message.hpp"
#include "MessageCodec.hpp"

int main()
{
    try
    {
        asio::io_context context;

        asio::ip::tcp::socket socket(context);

        asio::ip::tcp::endpoint endpoint(
            asio::ip::make_address("127.0.0.1"),
            5555
        );

        // 1. Connexion au serveur.

        socket.connect(endpoint);

        std::cout
            << "Connexion TCP etablie"
            << std::endl;

        // 2. Envoyer PING sans HELLO.

        Message ping(
            MessageType::Ping,
            {}
        );

        std::vector<std::uint8_t> buffer =
            MessageCodec::serialize(ping);

        asio::write(
            socket,
            asio::buffer(buffer)
        );

        std::cout
            << "PING envoye sans HELLO"
            << std::endl;

        // 3. Attendre une reponse ou une fermeture.
        // Un timer dans un thread ne suffit pas
        // a interrompre une lecture bloquante :
        // nous utilisons une lecture asynchrone.

        std::array<std::uint8_t, 1> response{};

        bool finished = false;
        bool closedByServer = false;
        bool receivedData = false;

        asio::steady_timer watchdog(context);

        watchdog.expires_after(
            std::chrono::seconds(3)
        );

        watchdog.async_wait(
            [&socket, &finished](
                const asio::error_code& error
            )
            {
                if (!error && !finished)
                {
                    asio::error_code ignored;
                    socket.cancel(ignored);
                }
            }
        );

        socket.async_read_some(
            asio::buffer(response),
            [&finished, &closedByServer, &receivedData, &watchdog](
                const asio::error_code& error,
                std::size_t bytesTransferred
            )
            {
                finished = true;
                watchdog.cancel();

                if (error == asio::error::eof ||
                    error == asio::error::connection_reset)
                {
                    closedByServer = true;
                }
                else if (!error && bytesTransferred > 0)
                {
                    receivedData = true;
                }
            }
        );

        context.run();

        if (receivedData)
        {
            std::cerr
                << "ECHEC : donnees recues avant fermeture"
                << std::endl;

            return 1;
        }

        if (!closedByServer)
        {
            std::cerr
                << "ECHEC : fermeture TCP non observee"
                << std::endl;

            return 1;
        }

        std::cout
            << "Fermeture TCP apres PING interdit : OK"
            << std::endl;

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Erreur : "
            << error.what()
            << std::endl;

        return 1;
    }
}
