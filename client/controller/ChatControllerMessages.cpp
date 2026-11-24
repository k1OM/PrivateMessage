#include "ChatController.hpp"

#include "AsyncNetworkClient.hpp"
#include "AsyncHeartbeatManager.hpp"

#include "Message.hpp"
#include "ChatPayloadCodec.hpp"

#include "ConnectionMessageHandler.hpp"
#include "PrivateMessageHandler.hpp"
#include "HeartbeatMessageHandler.hpp"

#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <vector>

void ChatController::handleIncomingMessage(
    const Message& message,
    std::weak_ptr<AsyncHeartbeatManager> weakHeartbeat
)
{
    switch (message.getType())
    {   
        case MessageType::HelloAck:
        {
            try
            {
                ConnectionMessageHandler handler;

                ChatEvent event =
                    handler.handle(message);

                pushEvent(
                    event.type,
                    event.text
                );

                if (auto manager = weakHeartbeat.lock())
                {
                    manager->start();
                }
            }
            catch (const std::exception& error)
            {
                pushEvent(
                    ChatEventType::Error,
                    error.what()
                );
            }

            break;
        }

        case MessageType::Pong:
        {
            if (auto manager = weakHeartbeat.lock())
            {
                try
                {
                    HeartbeatMessageHandler handler;

                    ChatEvent event =
                        handler.handle(message);

                    manager->onPong();

                    pushEvent(
                        event.type,
                        event.text
                    );
                }
                catch (const std::exception& error)
                {
                    pushEvent(
                        ChatEventType::Error,
                        error.what()
                    );
                }
            }

            break;
        }

        case MessageType::SessionAssigned:
        {
            try
            {
                ConnectionMessageHandler handler;

                ChatEvent event =
                    handler.handle(message);

                pushEvent(
                    event.type,
                    event.text
                );
            }
            catch (const std::exception& error)
            {
                pushEvent(
                    ChatEventType::Error,
                    error.what()
                );
            }

            break;
        }


        case MessageType::PrivateChat:
        {
            try
            {
                PrivateMessageHandler handler;

                ChatEvent event =
                    handler.handle(message);

                pushEvent(
                    event.type,
                    event.text,
                    event.senderId
                );
            }
            catch (const std::exception& error)
            {
                pushEvent(
                    ChatEventType::Error,
                    error.what()
                );
            }

            break;
        }

        case MessageType::Chat:
        {
            const std::vector<std::uint8_t>& payload =
                message.getPayload();

            std::string text(
                payload.begin(),
                payload.end()
            );

            pushEvent(
                ChatEventType::ChatMessage,
                text
            );

            break;
        }

        case MessageType::Error:
        {
            const std::vector<std::uint8_t>& payload =
                message.getPayload();

            std::string detail(
                payload.begin(),
                payload.end()
            );

            if (detail.empty())
            {
                detail = "Erreur serveur non precisee";
            }

            pushEvent(
                ChatEventType::ServerError,
                detail
            );

            break;
        }

        default:
        {
            pushEvent(
                ChatEventType::Error,
                "Message non gere par ChatController"
            );

            break;
        }
    }
}

void ChatController::sendPrivateMessage(
    std::uint64_t recipientId,
    const std::string& text
)
{
    if (recipientId == 0 || text.empty())
    {
        pushEvent(
            ChatEventType::Error,
            "Destinataire ou message invalide"
        );

        return;
    }

    if (!started.load() || stopping.load())
    {
        pushEvent(
            ChatEventType::Error,
            "Connexion indisponible ou en cours d'arret"
        );

        return;
    }


    std::uint64_t messageId = nextMessageId++;

    ChatPayload payload{
        ChatPayloadKind::PrivateRequest,
        recipientId,
        messageId,
        text
    };

    try
    {
        std::vector<std::uint8_t> data =
            ChatPayloadCodec::encode(payload);

        Message message(
            MessageType::PrivateChat,
            data
        );

        
		asio::post(
		    context,
		    [this, message]()
		    {
		        try
		        {
		            if (stopping.load() || !client || !client->isConnected())
		            {
		                pushEvent(
		                    ChatEventType::Error,
		                    "Client reseau indisponible"
		                );

		                return;
		            }

		            client->sendMessage(message);
		        }
		        catch (const std::exception& error)
		        {
		            pushEvent(
		                ChatEventType::Error,
		                error.what()
		            );
		        }
		    }
		);

    }
    catch (const std::exception& error)
    {
        pushEvent(
            ChatEventType::Error,
            error.what()
        );
    }
}