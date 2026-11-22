#include "ClientSession.hpp"

#include <memory>
#include <utility>

void ClientSession::sendMessage(const Message& message)
{
    if (!socket.is_open())
    {
        return;
    }

    bool writing = !outgoingMessages.empty();

    outgoingMessages.push_back(MessageCodec::serialize(message));

    if (!writing)
    {
        writeNext();
    }
}

void ClientSession::writeNext()
{
    if (outgoingMessages.empty() || !socket.is_open())
    {
        return;
    }

    std::shared_ptr<ClientSession> self = shared_from_this();

    asio::async_write(
        socket,
        asio::buffer(outgoingMessages.front()),
        [self](
            const asio::error_code& error,
            std::size_t bytesTransferred
        )
        {
            if (error)
            {
                self->closeConnection();
                return;
            }

            self->outgoingMessages.pop_front();

            if (!self->outgoingMessages.empty())
            {
                self->writeNext();
            }
        }
    );
}