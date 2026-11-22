
#include "SessionInactivityMonitor.hpp"

#include <asio.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <cstdint>

std::uint64_t generation = 0;

bool testExpiration()
{
    asio::io_context context;

    int timeoutCount = 0;

    std::shared_ptr<SessionInactivityMonitor> monitor =
        std::make_shared<SessionInactivityMonitor>(
            context.get_executor(),
            std::chrono::milliseconds(100)
        );

    monitor->setOnTimeout(
        [&timeoutCount]()
        {
            ++timeoutCount;
        }
    );

    monitor->start();
    context.run();

    return timeoutCount == 1;
}

bool testReset()
{
    asio::io_context context;

    int timeoutCount = 0;
    bool expiredTooEarly = false;

    std::shared_ptr<SessionInactivityMonitor> monitor =
        std::make_shared<SessionInactivityMonitor>(
            context.get_executor(),
            std::chrono::milliseconds(150)
        );

    asio::steady_timer resetTimer(context);
    asio::steady_timer checkTimer(context);

    monitor->setOnTimeout(
        [&timeoutCount]()
        {
            ++timeoutCount;
        }
    );

    resetTimer.expires_after(std::chrono::milliseconds(100));

    resetTimer.async_wait(
        [monitor](const asio::error_code& error)
        {
            if (!error)
            {
                monitor->reset();
            }
        }
    );

    checkTimer.expires_after(std::chrono::milliseconds(180));

    checkTimer.async_wait(
        [&timeoutCount, &expiredTooEarly](
            const asio::error_code& error
        )
        {
            if (!error && timeoutCount != 0)
            {
                expiredTooEarly = true;
            }
        }
    );

    monitor->start();
    context.run();

    return !expiredTooEarly && timeoutCount == 1;
}

bool testStop()
{
    asio::io_context context;

    int timeoutCount = 0;

    std::shared_ptr<SessionInactivityMonitor> monitor =
        std::make_shared<SessionInactivityMonitor>(
            context.get_executor(),
            std::chrono::milliseconds(150)
        );

    asio::steady_timer stopTimer(context);

    monitor->setOnTimeout(
        [&timeoutCount]()
        {
            ++timeoutCount;
        }
    );

    stopTimer.expires_after(std::chrono::milliseconds(50));

    stopTimer.async_wait(
        [monitor](const asio::error_code& error)
        {
            if (!error)
            {
                monitor->stop();
            }
        }
    );

    monitor->start();
    context.run();

    return timeoutCount == 0;
}


bool testStaleTimeout()
{
    asio::io_context context;

    int timeoutCount = 0;
    bool expiredTooEarly = false;

    std::shared_ptr<SessionInactivityMonitor> monitor =
        std::make_shared<SessionInactivityMonitor>(
            context.get_executor(),
            std::chrono::milliseconds(100)
        );

    monitor->setOnTimeout(
        [&timeoutCount]()
        {
            ++timeoutCount;
        }
    );

    // Premier timer : expiration initialement
    // prevue apres 100 ms.

    monitor->start();

    // Laisser passer l'ancienne echeance
    // sans executer context.run().

    std::this_thread::sleep_for(
        std::chrono::milliseconds(150)
    );

    // Une nouvelle activite repousse l'echeance.
    // Le nouveau timeout est prevu dans 100 ms.

    monitor->reset();

    // Traiter les callbacks deja disponibles.

    context.poll();

    if (timeoutCount != 0)
    {
        expiredTooEarly = true;
    }

    // Attendre la nouvelle expiration.

    context.restart();
    context.run();

    return !expiredTooEarly && timeoutCount == 1;
}


int main()
{
    if (!testExpiration())
    {
        std::cerr
            << "ECHEC : expiration"
            << std::endl;

        return 1;
    }

    std::cout << "Expiration : OK" << std::endl;

    if (!testReset())
    {
        std::cerr
            << "ECHEC : reset"
            << std::endl;

        return 1;
    }

    std::cout << "Reset : OK" << std::endl;

    if (!testStop())
    {
        std::cerr
            << "ECHEC : stop"
            << std::endl;

        return 1;
    }

    if (!testStaleTimeout())
    {
        std::cerr
            << "ECHEC : callback de timeout obsolete"
            << std::endl;

        return 1;
    }

    std::cout << "Callback obsolete : OK" << std::endl;

    std::cout << "Stop : OK" << std::endl;

    return 0;
}
