
#include "MultiClientTest.hpp"
#include "Protocol.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    constexpr std::size_t CLIENT_COUNT = 5;

    std::uint16_t port = Protocol::DEFAULT_PORT;

    if (argc > 2)
    {
        std::cerr << "Usage : test_multiple_clients [port]" << std::endl;
        return 1;
    }

    if (argc == 2)
    {
        try
        {
            const unsigned long parsedPort = std::stoul(argv[1]);

            if (parsedPort == 0 || parsedPort > 65535)
            {
                std::cerr << "Port invalide" << std::endl;
                return 1;
            }

            port = static_cast<std::uint16_t>(parsedPort);
        }
        catch (const std::exception& error)
        {
            std::cerr << "Port invalide : " << error.what() << std::endl;
            return 1;
        }
    }

    MultiClientTest test(
        "127.0.0.1",
        port,
        CLIENT_COUNT
    );

    if (!test.run())
    {
        std::cerr << "Test multiclient echoue" << std::endl;
        return 1;
    }

    std::cout
        << "Tous les clients ont termine avec succes !"
        << std::endl;

    return 0;
}
