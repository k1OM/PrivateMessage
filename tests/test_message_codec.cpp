
#include "Message.hpp"
#include "MessageCodec.hpp"
#include "Protocol.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void testSerialization()
{
    Message original(
        MessageType::Hello,
        {'H', 'e', 'l', 'l', 'o'}
    );

    std::vector<std::uint8_t> buffer =
        MessageCodec::serialize(original);

    require(buffer.size() == 10, "Taille incorrecte");

    require(buffer[0] == 0, "Longueur octet 1 incorrecte");
    require(buffer[1] == 0, "Longueur octet 2 incorrecte");
    require(buffer[2] == 0, "Longueur octet 3 incorrecte");
    require(buffer[3] == 5, "Longueur octet 4 incorrecte");

    require(buffer[4] == 1, "Type incorrect");

    Message copie = MessageCodec::deserialize(buffer);

    require(
        copie.getType() == MessageType::Hello,
        "Type deserialise incorrect"
    );

    require(
        copie.getPayload() == original.getPayload(),
        "Payload deserialise incorrect"
    );

    std::cout << "Serialization : OK" << std::endl;
}

void testInvalidHeader()
{
    std::vector<std::uint8_t> buffer =
    {
        0x00, 0x01, 0x00, 0x01, 0x05
    };

    bool rejected = false;

    try
    {
        MessageCodec::deserialize(buffer);
    }
    catch (const std::length_error&)
    {
        rejected = true;
    }

    require(rejected, "Message trop grand accepte");

    std::cout << "Limite payload : OK" << std::endl;
}

void testUnknownType()
{
    std::vector<std::uint8_t> buffer =
    {
        0x00, 0x00, 0x00, 0x00, 0xFF
    };

    bool rejected = false;

    try
    {
        MessageCodec::deserialize(buffer);
    }
    catch (const std::runtime_error&)
    {
        rejected = true;
    }

    require(rejected, "Type inconnu accepte");

    std::cout << "Type inconnu : OK" << std::endl;
}

void testIncompletePacket()
{
    std::vector<std::uint8_t> buffer =
    {
        0x00, 0x00, 0x00, 0x05, 0x01, 'H'
    };

    bool rejected = false;

    try
    {
        MessageCodec::deserialize(buffer);
    }
    catch (const std::runtime_error&)
    {
        rejected = true;
    }

    require(rejected, "Paquet incomplet accepte");

    std::cout << "Paquet incomplet : OK" << std::endl;
}

int main()
{
    try
    {
        testSerialization();
        testInvalidHeader();
        testUnknownType();
        testIncompletePacket();
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "ECHEC : " << error.what()
            << std::endl;

        return 1;
    }

    std::cout << "Tous les tests du codec ont reussi !"
              << std::endl;

    return 0;
}
