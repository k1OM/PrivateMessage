#include "AsyncHeartbeatManager.hpp"
#include "AsyncNetworkClient.hpp"

#include <iostream>
#include <exception>

void AsyncHeartbeatManager::schedulePing()
{
    if (!active)
    {
        return;
    }

    std::shared_ptr<AsyncHeartbeatManager> self =
        shared_from_this();

    pingTimer.expires_after(pingInterval);

    std::cout
        << "[DEBUG] Timer PING programme : "
        << pingInterval.count()
        << " ms"
        << std::endl;

    pingTimer.async_wait([self](const asio::error_code& error)
    {
        if (error == asio::error::operation_aborted)
        {
            return;
        }

        if (!self->active)
        {
            return;
        }

        if (error)
        {
            std::cerr
                << "Erreur timer PING : "
                << error.message()
                << std::endl;

            self->stop();

            if (auto client = self->client.lock())
            {
                client->disconnect();
            }

            return;
        }

        self->sendPing();
    });
}


void AsyncHeartbeatManager::startPongTimeout()
{
    std::shared_ptr<AsyncHeartbeatManager> self =
        shared_from_this();

    pongTimer.expires_after(pongTimeout);

    pongTimer.async_wait(
        [self](const asio::error_code& error)
        {
            if (error == asio::error::operation_aborted)
            {
                return;
            }

            if (!self->active || !self->waitingForPong)
            {
                return;
            }

            if (error)
            {
                std::cerr
                    << "Erreur timer PONG : "
                    << error.message()
                    << std::endl;
            }
            else
            {
                std::cerr
                    << "Heartbeat : PONG non recu"
                    << std::endl;
            }

            // Arreter les timers avant de notifier
            // le controleur.

            self->stop();

            if (self->onTimeoutCallback)
            {
                try
                {
                    self->onTimeoutCallback();
                }
                catch (const std::exception& exception)
                {
                    std::cerr
                        << "Erreur callback heartbeat : "
                        << exception.what()
                        << std::endl;
                }
                catch (...)
                {
                    std::cerr
                        << "Erreur inconnue callback heartbeat"
                        << std::endl;
                }
            }
        }
    );
}
