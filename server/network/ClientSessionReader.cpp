#include "ClientSession.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

void ClientSession::readHeader()
{
    std::shared_ptr<ClientSession> self = shared_from_this();

    asio::async_read(
        socket,
        asio::buffer(header),
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

            try
            {
                MessageCodec::validateHeader(self->header);

                std::uint32_t length =
                    MessageCodec::decodeLength(self->header);

                self->readPayload(length);
            }
            catch (const std::exception& exception)
            {
                std::cerr << exception.what() << std::endl;
                self->closeConnection();
            }
        }
    );
}

void ClientSession::readPayload(std::uint32_t length)
{
    payload.resize(length);

    std::shared_ptr<ClientSession> self = shared_from_this();

    auto handleMessage = [self]()
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

            Message message = MessageCodec::deserialize(buffer);

            self->inactivityMonitor->reset();

            self->messageHandler.processMessage(message);

            if (self->socket.is_open())
            {
                self->readHeader();
            }
        }
        catch (const std::exception& exception)
        {
            std::cerr << exception.what() << std::endl;
            self->closeConnection();
        }
    };

    if (length == 0)
    {
        handleMessage();
        return;
    }

    asio::async_read(
        socket,
        asio::buffer(payload),
        [self, handleMessage](
            const asio::error_code& error,
            std::size_t bytesTransferred
        )
        {
            if (error)
            {
                self->closeConnection();
                return;
            }

            handleMessage();
        }
    );
}