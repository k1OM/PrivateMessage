#include "AsyncNetworkClient.hpp"

#include <cstdint>
#include <exception>
#include <memory>
#include <vector>

void AsyncNetworkClient::readHeader()
{
    if (stopped || !connected)
    {
        return;
    }

    std::shared_ptr<AsyncNetworkClient> self =
        shared_from_this();

    asio::async_read(
        socket,
        asio::buffer(header),
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

            try
            {
                MessageCodec::validateHeader(
                    self->header
                );

                std::uint32_t length =
                    MessageCodec::decodeLength(
                        self->header
                    );

                self->readPayload(length);
            }
            catch (const std::exception& exception)
            {
                self->closeConnection(exception.what());
            }
        }
    );
}

void AsyncNetworkClient::readPayload(
    std::uint32_t length
)
{
    payload.resize(length);

    std::shared_ptr<AsyncNetworkClient> self =
        shared_from_this();

    auto completeMessage = [self]()
    {
        try
        {
            std::vector<std::uint8_t> buffer(
                self->header.begin(),
                self->header.end()
            );

            buffer.insert(
                buffer.end(),
                self->payload.begin(),
                self->payload.end()
            );

            Message message =
                MessageCodec::deserialize(buffer);

            self->handleMessage(message);

            if (!self->stopped)
            {
                self->readHeader();
            }
        }
        catch (const std::exception& exception)
        {
            self->closeConnection(exception.what());
        }
    };

    if (length == 0)
    {
        completeMessage();
        return;
    }

    asio::async_read(
        socket,
        asio::buffer(payload),
        [self, completeMessage](
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

            completeMessage();
        }
    );
}

void AsyncNetworkClient::handleMessage(
    const Message& message
)
{
    if (onMessage)
    {
        onMessage(message);
    }
}