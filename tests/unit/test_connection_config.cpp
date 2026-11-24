
#include "ConnectionConfig.hpp"

#include <cassert>
#include <iostream>

int main()
{
    // Test 1 : Adresse IPv4 valide
    ConnectionConfig configIPv4;

    configIPv4.host = "192.168.1.112";
    configIPv4.port = 5555;

    assert(configIPv4.isValid());

    // Test 2 : Nom de domaine valide
    ConnectionConfig configDomain;

    configDomain.host = "chat.example.com";
    configDomain.port = 5555;

    assert(configDomain.isValid());

    // Test 3 : Adresse vide
    ConnectionConfig configEmptyHost;

    configEmptyHost.host = "";
    configEmptyHost.port = 5555;

    assert(!configEmptyHost.isValid());

    // Test 4 : Port invalide
    ConnectionConfig configInvalidPort;

    configInvalidPort.host = "127.0.0.1";
    configInvalidPort.port = 0;

    assert(!configInvalidPort.isValid());

    // Test 5 : Configuration par defaut
    ConnectionConfig configDefault;

    assert(!configDefault.isValid());

    // Test 6 : Adresse IPv6
    ConnectionConfig configIPv6;

    configIPv6.host = "::1";
    configIPv6.port = 5555;

    assert(configIPv6.isValid());

    std::cout
        << "Tous les tests ConnectionConfig ont reussi !"
        << std::endl;

    return 0;
}
