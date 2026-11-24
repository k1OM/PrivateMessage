
#include "ChatController.hpp"
#include "ChatEvent.hpp"

#include <asio.hpp>

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    try
    {
        // Obtenir un port local temporaire.
        // Puis le liberer pour tester sans serveur.

        asio::io_context context;

        asio::ip::tcp::acceptor acceptor(
            context,
            asio::ip::tcp::endpoint(
                asio::ip::address_v4::loopback(),
                0
            )
        );

        const std::uint16_t port =
            acceptor.local_endpoint().port();

        acceptor.close();

        constexpr int iterations = 30;

        for (int i = 0; i < iterations; ++i)
        {
            ChatController controller;

            controller.start("127.0.0.1", port);

            // Arret immediat pendant la tentative.
            controller.stop();

            // Un envoi apres l'arret doit etre refuse.
            controller.sendPrivateMessage(
                1,
                "Message apres arret"
            );

            std::vector<ChatEvent> events =
                controller.pollEvents();

            bool sendRejected = false;

            for (const ChatEvent& event : events)
            {
                if (event.type == ChatEventType::Error &&
                    event.text ==
                        "Connexion indisponible ou en cours d'arret")
                {
                    sendRejected = true;
                }
            }

            if (!sendRejected)
            {
                std::cerr
                    << "[ECHEC] Envoi non refuse a l'iteration "
                    << i + 1
                    << std::endl;

                return 1;
            }

            // Le destructeur appellera stop() une
            // deuxieme fois : il doit rester sans effet.
        }

        std::cout
            << "[OK] "
            << iterations
            << " cycles start/stop termines"
            << std::endl;

        std::cout
            << "[OK] Envois apres arret refuses"
            << std::endl;

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "[ECHEC] "
            << error.what()
            << std::endl;

        return 1;
    }
}
