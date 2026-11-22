
#include "ChatServer.hpp"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        ChatServer server(5555);

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
