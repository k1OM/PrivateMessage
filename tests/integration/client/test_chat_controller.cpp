
#include "ChatController.hpp"
#include "Protocol.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <vector>

int main()
{
    ChatController controller;

    bool connected = false;
    bool ready = false;
    bool heartbeatReceived = false;
    bool disconnected = false;
    bool failed = false;

    try
    {
        controller.start(
            "127.0.0.1",
            Protocol::DEFAULT_PORT
        );

        std::cout
            << "Thread principal : toujours actif"
            << std::endl;

        auto deadline =
            std::chrono::steady_clock::now() +
            std::chrono::seconds(10);

        while (std::chrono::steady_clock::now() < deadline)
        {
            std::vector<ChatEvent> events =
                controller.pollEvents();

            for (const ChatEvent& event : events)
            {
                std::cout
                    << "[EVENT] "
                    << event.text
                    << std::endl;

                switch (event.type)
                {
                    case ChatEventType::Connected:
                    {
                        connected = true;
                        break;
                    }

                    case ChatEventType::Ready:
                    {
                        ready = true;
                        break;
                    }

                    case ChatEventType::Heartbeat:
                    {
                        heartbeatReceived = true;
                        break;
                    }

                    case ChatEventType::Error:
                    case ChatEventType::HeartbeatTimeout:
                    {
                        failed = true;
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }

            if (failed || heartbeatReceived)
            {
                break;
            }

            std::this_thread::sleep_for(
                std::chrono::milliseconds(50)
            );
        }

        controller.stop();

        std::vector<ChatEvent> remainingEvents =
            controller.pollEvents();

        for (const ChatEvent& event : remainingEvents)
        {
            std::cout
                << "[EVENT] "
                << event.text
                << std::endl;

            if (event.type == ChatEventType::Disconnected)
            {
                disconnected = true;
            }

            if (event.type == ChatEventType::Error ||
                event.type == ChatEventType::HeartbeatTimeout)
            {
                failed = true;
            }
        }
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Exception : "
            << error.what()
            << std::endl;

        return 1;
    }

    if (failed || !connected || !ready ||
        !heartbeatReceived || !disconnected)
    {
        std::cerr
            << "Test ChatController : ECHEC"
            << std::endl;

        return 1;
    }

    std::cout
        << "Test ChatController : OK"
        << std::endl;

    return 0;
}
