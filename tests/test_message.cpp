
#include "Message.hpp"

#include <stdexcept>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

int main()
{
    std::vector<std::uint8_t> contenu =
    {
        'H', 'e', 'l', 'l', 'o'
    };

    Message message(MessageType::Hello, contenu);

    std::vector<std::uint8_t> resultat = message.serialize();

    assert(resultat.size() == 10);

    // Longueur : 5 octets
    assert(resultat[0] == 0);
    assert(resultat[1] == 0);
    assert(resultat[2] == 0);
    assert(resultat[3] == 5);

    // Type : Hello = 1
    assert(resultat[4] == 1);

    // Premier octet du payload : 'H'
    assert(resultat[5] == 'H');

    std::cout << "Test Message : OK" << std::endl;

    Message copie = Message::deserialize(resultat);

    assert(copie.getType() == MessageType::Hello);
    assert(copie.getPayload() == contenu);

    std::cout << "Test Deserialize : OK" << std::endl;


bool erreurDetectee = false;

std::vector<std::uint8_t> paquetInvalide =
{
    0x00, 0x01, 0x00, 0x01, 0x05
};

try
{
    Message::deserialize(paquetInvalide);
}
catch (const std::length_error& e)
{
    erreurDetectee = true;

    std::cout << "Erreur detectee : "
              << e.what()
              << std::endl;
}

assert(erreurDetectee);

std::cout << "Test paquet invalide : OK" << std::endl;


    return 0;
}
