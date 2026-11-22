
#include "SessionInactivityMonitor.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

SessionInactivityMonitor::SessionInactivityMonitor(
    asio::any_io_executor executor,
    std::chrono::milliseconds timeout
)
    : timer(executor),
      timeout(timeout)
{
    if (timeout.count() <= 0)
    {
        throw std::invalid_argument(
            "Delai d'inactivite invalide"
        );
    }
}

void SessionInactivityMonitor::setOnTimeout(
    std::function<void()> callback
)
{
    onTimeout = std::move(callback);
}

void SessionInactivityMonitor::start()
{
    if (active)
    {
        return;
    }

    active = true;

    scheduleTimeout();
}

void SessionInactivityMonitor::reset()
{
    if (!active)
    {
        return;
    }

    scheduleTimeout();
}

void SessionInactivityMonitor::stop()
{
    active = false;

    ++generation;

    asio::error_code error;
    timer.cancel(error);
}


void SessionInactivityMonitor::scheduleTimeout()
{
    if (!active)
    {
        return;
    }

    ++generation;

    std::uint64_t currentGeneration = generation;

    std::shared_ptr<SessionInactivityMonitor> self =
        shared_from_this();

    timer.expires_after(timeout);

    timer.async_wait(
        [self, currentGeneration](const asio::error_code& error)
        {
            if (error == asio::error::operation_aborted)
            {
                return;
            }

            if (!self->active)
            {
                return;
            }

            // Ignorer les anciennes attentes.

            if (currentGeneration != self->generation)
            {
                return;
            }

            if (error)
            {
                std::cerr
                    << "Erreur timer inactivite : "
                    << error.message()
                    << std::endl;
            }
            else
            {
                std::cout
                    << "Timeout : session inactive"
                    << std::endl;
            }

            self->stop();

            if (self->onTimeout)
            {
                self->onTimeout();
            }
        }
    );
}

