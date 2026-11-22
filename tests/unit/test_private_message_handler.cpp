
#include "PrivateMessageHandler.hpp"
#include "ChatPayloadCodec.hpp"
#include "Message.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

int main()
{
    // 1. Construire un payload de livraison.

    ChatPayload payload{
        ChatPayloadKind::PrivateDelivery,
        2,
        42,
        "Bonjour depuis la session 2"
    };

    // 2. Encoder les donnees.

    std::vector<std::uint8_t> data =
        ChatPayloadCodec::encode(payload);

    // 3. Construire un message reseau.

    Message message(
        MessageType::PrivateChat,
        data
    );

    // 4. Executer notre handler.

    PrivateMessageHandler handler;

    ChatEvent event =
        handler.handle(message);

    // 5. Verifier le resultat.

    assert(event.type == ChatEventType::PrivateMessage);
    assert(event.senderId == 2);
    assert(event.text == "Bonjour depuis la session 2");

    std::cout
        << "PrivateMessageHandler : tests OK"
        << std::endl;

    return 0;
}
