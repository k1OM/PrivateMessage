
#include "ChatPayloadCodec.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

int main()
{
    // Test 1 : PrivateRequest.

    ChatPayload request{
        ChatPayloadKind::PrivateRequest,
        2,
        1001,
        "Salut Bob !"
    };

    std::vector<std::uint8_t> encoded =
        ChatPayloadCodec::encode(request);

    ChatPayload decoded =
        ChatPayloadCodec::decode(encoded);

    assert(decoded.kind == ChatPayloadKind::PrivateRequest);
    assert(decoded.sessionId == 2);
    assert(decoded.messageId == 1001);
    assert(decoded.text == "Salut Bob !");

    std::cout
        << "[OK] Encodage et decodage PrivateRequest"
        << std::endl;

    // Test 2 : PrivateDelivery.

    ChatPayload delivery{
        ChatPayloadKind::PrivateDelivery,
        1,
        1001,
        "Salut Bob !"
    };

    encoded = ChatPayloadCodec::encode(delivery);
    decoded = ChatPayloadCodec::decode(encoded);

    assert(decoded.kind == ChatPayloadKind::PrivateDelivery);
    assert(decoded.sessionId == 1);
    assert(decoded.messageId == 1001);
    assert(decoded.text == "Salut Bob !");

    std::cout
        << "[OK] Encodage et decodage PrivateDelivery"
        << std::endl;

    // Test 3 : encodage big-endian des identifiants.

    ChatPayload bigId{
        ChatPayloadKind::PrivateRequest,
        0x0102030405060708ULL,
        0x1112131415161718ULL,
        "Test"
    };

    encoded = ChatPayloadCodec::encode(bigId);

    assert(encoded.size() ==
        ChatPayloadCodec::HEADER_SIZE + bigId.text.size());

    for (std::size_t i = 0; i < 8; ++i)
    {
        assert(encoded[i + 2] == i + 1);
        assert(encoded[i + 10] == i + 0x11);
    }

    decoded = ChatPayloadCodec::decode(encoded);

    assert(decoded.sessionId == bigId.sessionId);
    assert(decoded.messageId == bigId.messageId);
    assert(decoded.text == bigId.text);

    std::cout
        << "[OK] Encodage big-endian des deux identifiants"
        << std::endl;

    // Test 4 : version incorrecte.

    encoded[0] = 99;

    bool rejected = false;

    try
    {
        ChatPayloadCodec::decode(encoded);
    }
    catch (const std::invalid_argument&)
    {
        rejected = true;
    }

    assert(rejected);

    std::cout
        << "[OK] Version invalide rejetee"
        << std::endl;

    // Test 5 : payload incomplet.

    rejected = false;

    try
    {
        ChatPayloadCodec::decode({
            ChatPayloadCodec::VERSION,
            1,
            0
        });
    }
    catch (const std::invalid_argument&)
    {
        rejected = true;
    }

    assert(rejected);

    std::cout
        << "[OK] Payload incomplet rejete"
        << std::endl;

    // Test 6 : identifiant de session nul.

    rejected = false;

    try
    {
        ChatPayloadCodec::encode({
            ChatPayloadKind::PrivateRequest,
            0,
            1001,
            "Test"
        });
    }
    catch (const std::invalid_argument&)
    {
        rejected = true;
    }

    assert(rejected);

    std::cout
        << "[OK] Session nulle rejetee"
        << std::endl;

    std::cout
        << "Test ChatPayloadCodec : OK"
        << std::endl;

    return 0;
}
