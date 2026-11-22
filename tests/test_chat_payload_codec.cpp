
#include "ChatPayloadCodec.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

int main()
{
    // Test 1 : Alice souhaite ecrire a Bob (session 2).

    ChatPayload request{
        ChatPayloadKind::PrivateRequest,
        2,
        "Salut Bob !"
    };

    std::vector<std::uint8_t> encoded =
        ChatPayloadCodec::encode(request);

    ChatPayload decoded =
        ChatPayloadCodec::decode(encoded);

    assert(decoded.kind == ChatPayloadKind::PrivateRequest);
    assert(decoded.sessionId == 2);
    assert(decoded.text == "Salut Bob !");

    std::cout
        << "[OK] Encodage et decodage PrivateRequest"
        << std::endl;

    // Test 2 : le serveur indique que l'expediteur
    // est Alice, session 1.

    ChatPayload delivery{
        ChatPayloadKind::PrivateDelivery,
        1,
        "Salut Bob !"
    };

    encoded = ChatPayloadCodec::encode(delivery);
    decoded = ChatPayloadCodec::decode(encoded);

    assert(decoded.kind == ChatPayloadKind::PrivateDelivery);
    assert(decoded.sessionId == 1);
    assert(decoded.text == "Salut Bob !");

    std::cout
        << "[OK] Encodage et decodage PrivateDelivery"
        << std::endl;

    // Test 3 : identifiant big-endian.

    ChatPayload bigId{
        ChatPayloadKind::PrivateRequest,
        0x0102030405060708ULL,
        "Test"
    };

    encoded = ChatPayloadCodec::encode(bigId);

    for (std::size_t i = 0; i < 8; ++i)
    {
        assert(encoded[i + 2] == i + 1);
    }

    std::cout
        << "[OK] Encodage big-endian sur 8 octets"
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
        ChatPayloadCodec::decode({1, 1, 0});
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
