
#include "ClientSession.hpp"

#include <iostream>
#include <utility>
#include <functional>
#include <chrono>
#include <stdexcept>


ClientSession::ClientSession(asio::ip::tcp::socket socket)
    : socket(std::move(socket)),
      inactivityMonitor(
          std::make_shared<SessionInactivityMonitor>(
              this->socket.get_executor(),
              std::chrono::seconds(TIMEOUT_SECONDS)
          )
      ),
      header{},
      payload{},
      outgoingMessages{},
      closed(false)
{
}


void ClientSession::start()
{
    std::cout
        << "Session demarree"
        << std::endl;

    std::weak_ptr<ClientSession> weakSession =
        weak_from_this();

    // 1. Configurer le gestionnaire du protocole

    messageHandler.setSessionId(sessionId);

    messageHandler.setOnSend(
        [weakSession](const Message& message)
        {
            std::shared_ptr<ClientSession> session =
                weakSession.lock();

            if (session && !session->closed)
            {
                session->sendMessage(message);
            }
        }
    );

    messageHandler.setOnClose(
        [weakSession]()
        {
            std::shared_ptr<ClientSession> session =
                weakSession.lock();

            if (session)
            {
                session->closeConnection();
            }
        }
    );

    messageHandler.setOnChat(
        [weakSession](const Message& message)
        {
            std::shared_ptr<ClientSession> session =
                weakSession.lock();

            if (session && session->onChat)
            {
                session->onChat(message);
            }
        }
    );

    messageHandler.setOnPrivateChat(
        [weakSession](const Message& message)
        {
            std::shared_ptr<ClientSession> session =
                weakSession.lock();

            if (session && session->onPrivateChat)
            {
                session->onPrivateChat(message);
            }
        }
    );

    // 2. Configurer le moniteur d'inactivite

    inactivityMonitor->setOnTimeout(
        [weakSession]()
        {
            std::shared_ptr<ClientSession> session =
                weakSession.lock();

            if (!session || session->closed)
            {
                return;
            }

            std::cout
                << "Timeout : client inactif pendant "
                << TIMEOUT_SECONDS
                << " secondes"
                << std::endl;

            session->closeConnection();
        }
    );

    // 3. Demarrer la surveillance et la lecture TCP

    inactivityMonitor->start();

    readHeader();
}


void ClientSession::stop()
{
    closeConnection();
}

void ClientSession::setOnClose(
    std::function<void()> callback
)
{
    onClose = std::move(callback);
}

void ClientSession::setOnChat(
    std::function<void(const Message&)> callback
)
{
    onChat = std::move(callback);
}

void ClientSession::setSessionId(std::uint64_t id)
{
    if (id == 0)
    {
        throw std::invalid_argument(
            "Identifiant de session invalide"
        );
    }

    sessionId = id;
}

void ClientSession::setOnPrivateChat(
    std::function<void(const Message&)> callback
)
{
    onPrivateChat = std::move(callback);
}