
#include "ChatServer.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    try
    {
        std::uint16_t port = 5555;

        if (argc > 2)
        {
            throw std::invalid_argument(
                "Usage : privatechat-server [port]"
            );
        }

        if (argc == 2)
        {
            std::string argument = argv[1];
            std::size_t position = 0;

            unsigned long value = std::stoul(
                argument,
                &position
            );

            if (position != argument.size() ||
                value == 0 ||
                value > 65535)
            {
                throw std::invalid_argument(
                    "Port invalide : valeur attendue entre 1 et 65535"
                );
            }

            port = static_cast<std::uint16_t>(value);
        }

        ChatServer server(port);

        server.start();
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Erreur serveur : "
            << error.what()
            << std::endl;

        return 1;
    }

    return 0;
}
