
#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstddef>
#include <cstdint>

namespace Protocol
{
    constexpr std::uint16_t DEFAULT_PORT = 5555;

    constexpr std::size_t HEADER_SIZE = 5;

    constexpr std::uint32_t MAX_PAYLOAD = 65536;

    constexpr std::uint8_t MIN_MESSAGE_TYPE = 1;
    constexpr std::uint8_t MAX_MESSAGE_TYPE = 8;
}

#endif
