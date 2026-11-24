#include "AsyncNetworkClient.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>


void AsyncNetworkClient::connect(
    const std::string& address,
    std::uint16_t port
)
{
    if (stopped || socket.is_open())
    {
        throw std::runtime_error(
            "Client deja utilise"
        );
    }

    std::shared_ptr<AsyncNetworkClient> self =
        shared_from_this();

    resolver.async_resolve(
        address,
        std::to_string(port),

        [self](
            const asio::error_code& error,
            asio::ip::tcp::resolver::results_type endpoints
        )
        {
            if (self->stopped)
            {
                return;
            }

            if (error)
            {
                self->closeConnection(
                    "Resolution DNS impossible : " +
                    error.message()
                );

                return;
            }

            asio::async_connect(
                self->socket,
                endpoints,

                [self](
                    const asio::error_code& error,
                    const asio::ip::tcp::endpoint&
                )
                {
                    if (self->stopped)
                    {
                        return;
                    }

                    if (error)
                    {
                        self->closeConnection(
                            "Connexion impossible : " +
                            error.message()
                        );

                        return;
                    }

                    self->connected = true;

                    std::cout
                        << "Connexion asynchrone etablie"
                        << std::endl;

                    self->readHeader();

                    if (self->onConnect)
                    {
                        self->onConnect();
                    }
                }
            );
        }
    );
}


void AsyncNetworkClient::disconnect()
{
    closeConnection("Deconnexion demandee");
}


void AsyncNetworkClient::closeConnection(
    const std::string& reason
)
{
    if (stopped)
    {
        return;
    }

    stopped = true;
    connected = false;

    asio::error_code error;

    // Annuler la resolution DNS en cours
    resolver.cancel();

    // Annuler les operations reseau
    socket.cancel(error);

    socket.shutdown(
        asio::ip::tcp::socket::shutdown_both,
        error
    );

    socket.close(error);

    // Ne pas detruire les buffers d'une ecriture
    // asynchrone encore en attente de son callback.

    std::cout
        << "Connexion fermee : "
        << reason
        << std::endl;

    if (onDisconnect)
    {
        onDisconnect(reason);
    }
}
