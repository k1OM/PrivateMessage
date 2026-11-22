
#include "NetworkClient.hpp"
#include "MessageCodec.hpp"

#include <array>
#include <stdexcept>
#include <vector>

NetworkClient::NetworkClient()
    : ioContext(),
      socket(ioContext),
      connected(false)
{
}

NetworkClient::~NetworkClient()
{
    disconnect();
}

void NetworkClient::connect(
    const std::string& address,
    std::uint16_t port
)
{
    if (connected)
    {
        throw std::runtime_error(
            "Client deja connecte"
        );
    }

    asio::ip::tcp::resolver resolver(ioContext);

    asio::ip::tcp::resolver::results_type endpoints =
        resolver.resolve(address, std::to_string(port));

    asio::connect(socket, endpoints);

    connected = true;
}

void NetworkClient::disconnect()
{
    if (!socket.is_open())
    {
        connected = false;
        return;
    }

    asio::error_code error;

    socket.shutdown(
        asio::ip::tcp::socket::shutdown_both,
        error
    );

    socket.close(error);

    connected = false;
}


void NetworkClient::sendMessage(const Message& message)
{
    if (!connected)
    {
        throw std::runtime_error("Client non connecte");
    }

    std::vector<std::uint8_t> buffer =
        MessageCodec::serialize(message);

    try
    {
        asio::write(
            socket,
            asio::buffer(buffer)
        );
    }
    catch (...)
    {
        disconnect();
        throw;
    }
}



Message NetworkClient::receiveMessage()
{
    if (!connected)
    {
        throw std::runtime_error("Client non connecte");
    }

    try
    {
        MessageCodec::Header header{};

        asio::read(
            socket,
            asio::buffer(header)
        );

        MessageCodec::validateHeader(header);

        std::uint32_t length =
            MessageCodec::decodeLength(header);

        std::vector<std::uint8_t> buffer(
            header.begin(),
            header.end()
        );

        std::vector<std::uint8_t> payload(length);

        if (length > 0)
        {
            asio::read(
                socket,
                asio::buffer(payload)
            );

            buffer.insert(
                buffer.end(),
                payload.begin(),
                payload.end()
            );
        }

        return MessageCodec::deserialize(buffer);
    }
    catch (...)
    {
        disconnect();
        throw;
    }
}


bool NetworkClient::isConnected() const
{
    return connected;
}
