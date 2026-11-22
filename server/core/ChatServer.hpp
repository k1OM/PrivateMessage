
#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include "SessionManager.hpp"

#include <asio.hpp>
#include <cstdint>
#include <memory>

class ChatServer
{
private:
    asio::io_context ioContext;

    asio::ip::tcp::acceptor acceptor;

    std::uint16_t port;

    std::shared_ptr<SessionManager> sessionManager;

public:
    explicit ChatServer(std::uint16_t port);

    void start();

    void stop();

private:
    void acceptConnection();
};

#endif
