#include "ClientSession.hpp"

#include <chrono>
#include <iostream>
#include <memory>

void ClientSession::startTimeout()
{
    if (closed)
    {
        return;
    }

    std::shared_ptr<ClientSession> self = shared_from_this();

    inactivityTimer.expires_after(
        std::chrono::seconds(TIMEOUT_SECONDS)
    );

    inactivityTimer.async_wait(
        [self](const asio::error_code& error)
        {
            if (error == asio::error::operation_aborted)
            {
                return;
            }

            if (error)
            {
                std::cerr
                    << "Erreur timer : "
                    << error.message()
                    << std::endl;

                self->closeConnection();
                return;
            }

            if (!self->closed)
            {
                std::cout
                    << "Timeout : client inactif pendant "
                    << TIMEOUT_SECONDS
                    << " secondes"
                    << std::endl;

                self->closeConnection();
            }
        }
    );
}

void ClientSession::resetTimeout()
{
    if (closed)
    {
        return;
    }

    startTimeout();
}