
#include "Message.hpp"
#include "MessageCodec.hpp"

#include <asio.hpp>

#include <cstdint>
#include <exception>
#include <iostream>
#include <vector>

int main()
{
    try
    {
        asio::io_context context;

        asio::ip::tcp::acceptor acceptor(
            context,
            asio::ip::tcp::endpoint(
                asio::ip::make_address("127.0.0.1"),
                5556
            )
        );

        std::cout
            << "Serveur silencieux : port 5556"
            << std::endl;

        asio::ip::tcp::socket socket(context);

        acceptor.accept(socket);

        std::cout << "Client connecte" << std::endl;

        // Lire le premier message : HELLO
        MessageCodec::Header header{};

        asio::read(
            socket,
            asio::buffer(header)
        );

        MessageCodec::validateHeader(header);

        std::uint32_t length =
            MessageCodec::decodeLength(header);

        if (header[4] !=
                static_cast<std::uint8_t>(MessageType::Hello) ||
            length != 0)
        {
            throw std::runtime_error("HELLO attendu");
        }

        // Repondre avec HELLO_ACK
        Message response(MessageType::HelloAck, {});

        std::vector<std::uint8_t> buffer =
            MessageCodec::serialize(response);

        asio::write(
            socket,
            asio::buffer(buffer)
        );

        std::cout << "HELLO_ACK envoye" << std::endl;

        // Lire le PING suivant
        asio::read(
            socket,
            asio::buffer(header)
        );

        MessageCodec::validateHeader(header);

        length = MessageCodec::decodeLength(header);

        if (header[4] !=
                static_cast<std::uint8_t>(MessageType::Ping) ||
            length != 0)
        {
            throw std::runtime_error("PING attendu");
        }

        std::cout << "PING recu" << std::endl;
        std::cout << "PONG volontairement ignore" << std::endl;

        // Garder le socket ouvert sans repondre.
        // Attendre que le client se deconnecte.

        std::uint8_t byte = 0;
        asio::error_code error;

        asio::read(
            socket,
            asio::buffer(&byte, 1),
            error
        );

        if (error == asio::error::eof ||
            error == asio::error::connection_reset)
        {
            std::cout
                << "Deconnexion du client detectee"
                << std::endl;

            return 0;
        }

        std::cerr
            << "Fin inattendue : "
            << error.message()
            << std::endl;

        return 1;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Erreur serveur : "
            << error.what()
            << std::endl;

        return 1;
    }
}
