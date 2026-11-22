
#include "ChatServer.hpp"
#include "ClientSession.hpp"
#include "SessionManager.hpp"
#include "ChatPayloadCodec.hpp"
#include "Message.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <exception>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

ChatServer::ChatServer(std::uint16_t port)
    : ioContext(),
      acceptor(
          ioContext,
          asio::ip::tcp::endpoint(
              asio::ip::address_v4::any(),
              port
          )
      ),
      port(port),
      sessionManager(std::make_shared<SessionManager>())
{
}

void ChatServer::start()
{
    asio::ip::tcp::endpoint endpoint = acceptor.local_endpoint();

    std::cout << "PrivateChat Server" << std::endl;

    std::cout << "Ecoute sur "
              << endpoint.address().to_string()
              << ":"
              << endpoint.port()
              << std::endl;

    acceptConnection();

    ioContext.run();
}

void ChatServer::stop()
{
    asio::error_code error;
    acceptor.close(error);
    ioContext.stop();
}



void ChatServer::acceptConnection()
{
    acceptor.async_accept(
        [this](
            const asio::error_code& error,
            asio::ip::tcp::socket socket
        )
        {
            if (!error)
            {
                if (!sessionManager->hasCapacity())
                {
                    std::cout
                        << "Limite de connexions atteinte"
                        << std::endl;

                    asio::error_code closeError;
                    socket.close(closeError);
                }
                
                else
                {
                    std::shared_ptr<ClientSession> session = std::make_shared<ClientSession>(std::move(socket));

                    SessionManager::SessionId id = sessionManager->add(session);

                    session->setSessionId(id);

                    std::weak_ptr<SessionManager> weakManager = sessionManager;

                    // Reference faible vers la session emettrice
                    std::weak_ptr<ClientSession> weakSession = session;


                    session->setOnPrivateChat(
                        [weakManager, weakSession, id](
                            const Message& message
                        )
                        {
                            std::shared_ptr<SessionManager> manager =
                                weakManager.lock();

                            std::shared_ptr<ClientSession> sender =
                                weakSession.lock();

                            if (!manager || !sender)
                            {
                                return;
                            }

                            try
                            {
                                // Decoder la demande du client.
                                ChatPayload request =
                                    ChatPayloadCodec::decode(
                                        message.getPayload()
                                    );

                                if (request.kind !=
                                    ChatPayloadKind::PrivateRequest)
                                {
                                    throw std::invalid_argument(
                                        "PrivateRequest attendu"
                                    );
                                }

                                // Construire le message de livraison.
                                // L'expediteur est identifie par le serveur.
                                
                                ChatPayload delivery{
                                    ChatPayloadKind::PrivateDelivery,
                                    id,
                                    request.messageId,
                                    request.text
                                };


                                std::vector<std::uint8_t> payload =
                                    ChatPayloadCodec::encode(delivery);

                                Message outgoing(
                                    MessageType::PrivateChat,
                                    payload
                                );

                                // Envoyer uniquement au destinataire.
                                bool sent = manager->sendTo(
                                    request.sessionId,
                                    outgoing
                                );

                                if (!sent)
                                {
                                    throw std::runtime_error(
                                        "Destinataire introuvable"
                                    );
                                }

                                std::cout
                                    << "Message prive : session "
                                    << id
                                    << " -> session "
                                    << request.sessionId
                                    << std::endl;
                            }
                            catch (const std::exception& error)
                            {
                                std::cerr
                                    << "Erreur message prive : "
                                    << error.what()
                                    << std::endl;

                                std::string detail = error.what();

                                std::vector<std::uint8_t> errorPayload(
                                    detail.begin(),
                                    detail.end()
                                );

                                sender->sendMessage(
                                    Message(
                                        MessageType::Error,
                                        errorPayload
                                    )
                                );
                            }
                        }
                    );


                    // Callback pour diffuser les messages CHAT
                    session->setOnChat(
                        [weakManager, weakSession](const Message& message)
                        {
                            std::shared_ptr<SessionManager> manager =  weakManager.lock();

                            std::shared_ptr<ClientSession> sender = weakSession.lock();

                            if (!manager || !sender)
                            {
                                return;
                            }


                            manager->broadcastExcept(sender, message);
                        }
                    );

                    // Important : conserver le nettoyage des sessions
                    session->setOnClose(
                        [weakManager, id]()
                        {
                            if (auto manager = weakManager.lock())
                            {
                                manager->remove(id);
                            }
                        }
                    );

                    std::cout
                        << "Session " << id
                        << " connectee. Actives : "
                        << sessionManager->count()
                        << std::endl;

                    session->start();
                }

            }
            else if (error != asio::error::operation_aborted)
            {
                std::cerr
                    << "Erreur acceptation : "
                    << error.message()
                    << std::endl;
            }

            if (acceptor.is_open())
            {
                acceptConnection();
            }
        }
    );
}


