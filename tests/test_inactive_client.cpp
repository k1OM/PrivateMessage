
#include <asio.hpp>

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    try
    {
        asio::io_context context;
        asio::ip::tcp::socket socket(context);

        asio::ip::tcp::endpoint endpoint(
            asio::ip::make_address("127.0.0.1"),
            5555
        );

        socket.connect(endpoint);

        std::cout
            << "Connexion TCP etablie."
            << std::endl;

        std::cout
            << "Aucun message ne sera envoye."
            << std::endl;

        std::this_thread::sleep_for(
            std::chrono::seconds(20)
        );

        std::cout
            << "Fin du test."
            << std::endl;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Erreur : "
            << error.what()
            << std::endl;

        return 1;
    }

    return 0;
}
