
#include "MessageCodec.hpp"

#include <stdexcept>

std::vector<std::uint8_t> MessageCodec::serialize(
    const Message& message
)
{
    const auto& payload = message.getPayload();

    if (payload.size() > Protocol::MAX_PAYLOAD)
    {
        throw std::length_error("Payload trop grand");
    }

    std::uint8_t type =
        static_cast<std::uint8_t>(message.getType());

    if (type < Protocol::MIN_MESSAGE_TYPE ||
        type > Protocol::MAX_MESSAGE_TYPE)
    {
        throw std::runtime_error("Type invalide");
    }

    std::uint32_t length =
        static_cast<std::uint32_t>(payload.size());

    std::vector<std::uint8_t> buffer;

    buffer.reserve(
        Protocol::HEADER_SIZE + payload.size()
    );

    buffer.push_back(
        static_cast<std::uint8_t>((length >> 24) & 0xFF)
    );

    buffer.push_back(
        static_cast<std::uint8_t>((length >> 16) & 0xFF)
    );

    buffer.push_back(
        static_cast<std::uint8_t>((length >> 8) & 0xFF)
    );

    buffer.push_back(
        static_cast<std::uint8_t>(length & 0xFF)
    );

    buffer.push_back(type);

    buffer.insert(
        buffer.end(),
        payload.begin(),
        payload.end()
    );

    return buffer;
}

std::uint32_t MessageCodec::decodeLength(
    const Header& header
)
{
    return
        (static_cast<std::uint32_t>(header[0]) << 24) |
        (static_cast<std::uint32_t>(header[1]) << 16) |
        (static_cast<std::uint32_t>(header[2]) << 8) |
        static_cast<std::uint32_t>(header[3]);
}

void MessageCodec::validateHeader(
    const Header& header
)
{
    std::uint32_t length = decodeLength(header);

    if (length > Protocol::MAX_PAYLOAD)
    {
        throw std::length_error("Payload trop grand");
    }

    std::uint8_t type = header[4];

    if (type < Protocol::MIN_MESSAGE_TYPE ||
        type > Protocol::MAX_MESSAGE_TYPE)
    {
        throw std::runtime_error("Type inconnu");
    }
}

Message MessageCodec::deserialize(
    const std::vector<std::uint8_t>& buffer
)
{
    if (buffer.size() < Protocol::HEADER_SIZE)
    {
        throw std::runtime_error("En-tete incomplet");
    }

    Header header{};

    for (std::size_t i = 0; i < header.size(); ++i)
    {
        header[i] = buffer[i];
    }

    validateHeader(header);

    std::uint32_t length = decodeLength(header);

    if (buffer.size() != Protocol::HEADER_SIZE + length)
    {
        throw std::runtime_error("Taille invalide");
    }

    MessageType type =
        static_cast<MessageType>(header[4]);

    std::vector<std::uint8_t> payload(
        buffer.begin() + Protocol::HEADER_SIZE,
        buffer.end()
    );

    return Message(type, payload);
}
