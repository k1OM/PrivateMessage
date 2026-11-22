#include "AsyncNetworkClient.hpp"

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

void AsyncNetworkClient::sendMessage(
    const Message& message
)
{
    if (!connected || stopped)
    {
        throw std::runtime_error(
            "Client non connecte"
        );
    }

    std::vector<std::uint8_t> buffer =
        MessageCodec::serialize(message);

    bool writing = !outgoingMessages.empty();

    outgoingMessages.push_back(std::move(buffer));

    if (!writing)
    {
        writeNext();
    }
}

void AsyncNetworkClient::writeNext()
{
    if (stopped || outgoingMessages.empty())
    {
        return;
    }

    std::shared_ptr<AsyncNetworkClient> self =
        shared_from_this();

    asio::async_write(
        socket,
        asio::buffer(outgoingMessages.front()),
        [self](
            const asio::error_code& error,
            std::size_t bytesTransferred
        )
        {
            if (self->stopped)
            {
                return;
            }

            if (error)
            {
                self->closeConnection(error.message());
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