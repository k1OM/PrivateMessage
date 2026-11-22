#include "ChatController.hpp"

#include "AsyncNetworkClient.hpp"
#include "AsyncHeartbeatManager.hpp"
#include "Message.hpp"
#include "ChatPayloadCodec.hpp"

#include "ConnectionMessageHandler.hpp"
#include "PrivateMessageHandler.hpp"
#include "HeartbeatMessageHandler.hpp"

#include <chrono>
#include <cstdint>
#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

ChatController::ChatController()
    : context(),
      workGuard(asio::make_work_guard(context)),
      started(false),
      stopping(false)
{
}

ChatController::~ChatController()
{
    stop();
}

void ChatController::pushEvent(
    ChatEventType type,
    const std::string& text
)
{
    std::lock_guard<std::mutex> lock(eventMutex);

    events.push({
        type,
        text
    });
}


void ChatController::pushEvent(
    ChatEventType type,
    const std::string& text,
    std::uint64_t senderId
)
{
    std::lock_guard<std::mutex> lock(eventMutex);

    events.push({
        type,
        text,
        senderId
    });
}


std::vector<ChatEvent> ChatController::pollEvents()
{
    std::vector<ChatEvent> result;

    std::lock_guard<std::mutex> lock(eventMutex);

    while (!events.empty())
    {
        result.push_back(std::move(events.front()));
        events.pop();
    }

    return result;
}

void ChatController::start(
    const std::string& address,
    std::uint16_t port
)
{
    if (started.exchange(true))
    {
        throw std::runtime_error(
            "ChatController deja demarre"
        );
    }

    try
    {
        client = std::make_shared<AsyncNetworkClient>(
            context
        );

        heartbeat = std::make_shared<AsyncHeartbeatManager>(
            context,
            client,
            std::chrono::seconds(5),
            std::chrono::seconds(3)
        );

        configureCallbacks();

        // Initialisation effectuee avant le lancement
        // de l'unique thread executant io_context.
        client->connect(address, port);

        networkThread = std::thread(
            [this]()
            {
                context.run();
            }
        );
    }
    catch (...)
    {
        workGuard.reset();

        if (networkThread.joinable())
        {
            networkThread.join();
        }

        heartbeat.reset();
        client.reset();

        stopping = true;
        throw;
    }
}

void ChatController::stop()
{
    if (!started || stopping.exchange(true))
    {
        return;
    }

    asio::post(
        context,
        [this]()
        {
            if (heartbeat)
            {
                heartbeat->stop();
            }

            if (client)
            {
                client->disconnect();
            }
        }
    );

    workGuard.reset();

    if (networkThread.joinable())
    {
        networkThread.join();
    }

    heartbeat.reset();
    client.reset();
}