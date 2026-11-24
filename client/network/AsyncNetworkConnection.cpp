#include "AsyncNetworkClient.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>


void AsyncNetworkClient::connect(
    const std::string& address,
    std::uint16_t port
)
{
    if (connectionState != ConnectionState::Idle)
    {
        throw std::runtime_error("Client deja utilise ou connexion en cours");
    }

    std::shared_ptr<AsyncNetworkClient> self = shared_from_this();

    connectionState = ConnectionState::Resolving;

    startConnectionTimeout(
        ConnectionState::Resolving,
        dnsTimeout,
        "Timeout : resolution DNS"
    );

    resolver.async_resolve(
        address,
        std::to_string(port),

        [self](
            const asio::error_code& error,
            asio::ip::tcp::resolver::results_type endpoints
        )
        {
            if (self->connectionState != ConnectionState::Resolving)
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

            self->connectionTimer.cancel();
            self->connectionState = ConnectionState::Connecting;

            asio::async_connect(
                self->socket,
                endpoints,

                [self](
                    const asio::error_code& error,
                    const asio::ip::tcp::endpoint&
                )
                {
                    if (self->connectionState != ConnectionState::Connecting)
                    {
                        return;
                    }

                    if (error)
                    {
                        self->closeConnection("Connexion impossible : " + error.message());

                        return;
                    }

                    self->connectionTimer.cancel();

                    self->connected = true;
                    self->connectionState = ConnectionState::Connected;

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

            self->startConnectionTimeout(
                ConnectionState::Connecting,
                self->tcpTimeout,
                "Timeout : connexion TCP"
            );
        }
    );
}

void AsyncNetworkClient::startConnectionTimeout(
    ConnectionState expectedState,
    std::chrono::steady_clock::duration duration,
    const std::string& reason
)
{
    connectionTimer.expires_after(duration);

    std::shared_ptr<AsyncNetworkClient> self =
        shared_from_this();

    connectionTimer.async_wait(
        [self, expectedState, reason](
            const asio::error_code& error
        )
        {
            if (error == asio::error::operation_aborted)
            {
                return;
            }

            if (error)
            {
                self->closeConnection(
                    "Erreur timer : " + error.message()
                );

                return;
            }

            self->closeConnection(reason);
        }
    );
}

void AsyncNetworkClient::disconnect()
{
    closeConnection("Deconnexion demandee");
}

void AsyncNetworkClient::closeConnection(const std::string& reason)
{
    if (stopped)
    {
        return;
    }

    stopped = true;
    connected = false;
    connectionState = ConnectionState::Stopped;

    asio::error_code error;

    // Annuler le timeout de connexion
    connectionTimer.cancel();

    // Annuler la resolution DNS
    resolver.cancel();

    // Annuler les operations TCP
    socket.cancel(error);

    socket.shutdown(
        asio::ip::tcp::socket::shutdown_both,
        error
    );

    socket.close(error);

    // Conserver les buffers des ecritures
    // asynchrones jusqu'a leurs callbacks.

    std::cout
        << "Connexion fermee : "
        << reason
        << std::endl;

    if (onDisconnect)
    {
        onDisconnect(reason);
    }
}
