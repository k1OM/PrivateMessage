
#include "ChatPayloadCodec.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>


int main()
{
    ChatPayload original{
        ChatPayloadKind::PrivateRequest,
        2,
        42,
        "Bonjour session 2 !"
    };

    std::vector<std::uint8_t> encoded =
        ChatPayloadCodec::encode(original);

    assert(encoded.size() ==
        ChatPayloadCodec::HEADER_SIZE +
        original.text.size());

    assert(encoded[0] ==
        ChatPayloadCodec::VERSION);

    assert(encoded[1] ==
        static_cast<std::uint8_t>(
            ChatPayloadKind::PrivateRequest
        ));

    ChatPayload decoded =
        ChatPayloadCodec::decode(encoded);

    assert(decoded.kind == original.kind);
    assert(decoded.sessionId == original.sessionId);
    assert(decoded.messageId == original.messageId);
    assert(decoded.text == original.text);

    ChatPayload accepted{
        ChatPayloadKind::PrivateAccepted,
        2,
        42,
        ""
    };

    ChatPayload acceptedDecoded =
        ChatPayloadCodec::decode(
            ChatPayloadCodec::encode(accepted)
        );

    assert(acceptedDecoded.messageId == 42);
    assert(acceptedDecoded.text.empty());

    std::cout
        << "ChatPayloadCodec v2 : tests OK"
        << std::endl;

    return 0;
}
