
#ifndef NETWORKCLIENT_HPP
#define NETWORKCLIENT_HPP

#include <asio.hpp>

#include <cstdint>
#include <string>

#include "Message.hpp"

class NetworkClient
{
private:
    asio::io_context ioContext;

    asio::ip::tcp::socket socket;

    bool connected;

public:
    NetworkClient();

    ~NetworkClient();

    void connect(
        const std::string& address,
        std::uint16_t port
    );

    void disconnect();

    void sendMessage(const Message& message);

    Message receiveMessage();

    bool isConnected() const;
};

#endif
