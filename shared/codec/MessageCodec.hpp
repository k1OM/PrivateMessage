
#ifndef MESSAGECODEC_HPP
#define MESSAGECODEC_HPP

#include "Message.hpp"
#include "Protocol.hpp"

#include <array>
#include <cstdint>
#include <vector>

class MessageCodec
{
public:
    using Header = std::array<
        std::uint8_t,
        Protocol::HEADER_SIZE
    >;

    static std::vector<std::uint8_t> serialize(
        const Message& message
    );

    static Message deserialize(
        const std::vector<std::uint8_t>& buffer
    );

    static std::uint32_t decodeLength(
        const Header& header
    );

    static void validateHeader(
        const Header& header
    );
};

#endif
