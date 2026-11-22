
#include "PrivateMessageHandler.hpp"

#include "ChatPayloadCodec.hpp"

#include <stdexcept>
#include <utility>

ChatEvent PrivateMessageHandler::handle(
    const Message& message
) const
{
    // 1. Verifier le type du message reseau.

    if (message.getType() != MessageType::PrivateChat)
    {
        throw std::invalid_argument(
            "PrivateChat attendu"
        );
    }

    // 2. Decoder le payload.

    ChatPayload payload =
        ChatPayloadCodec::decode(
            message.getPayload()
        );

    // 3. Verifier sa nature.

    if (payload.kind != ChatPayloadKind::PrivateDelivery)
    {
        throw std::invalid_argument(
            "PrivateDelivery attendu"
        );
    }

    // 4. Construire un evenement applicatif.

    ChatEvent event{
        ChatEventType::PrivateMessage,
        std::move(payload.text),
        payload.sessionId
    };

    return event;
}
