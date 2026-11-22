
#ifndef CHATPAYLOADCODEC_HPP
#define CHATPAYLOADCODEC_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

enum class ChatPayloadKind : std::uint8_t
{
    PrivateRequest = 1,
    PrivateDelivery = 2,
    PrivateAccepted = 3,
    PrivateRejected = 4
};

struct ChatPayload
{
    ChatPayloadKind kind;
    std::uint64_t sessionId;
    std::uint64_t messageId;
    std::string text;
};

class ChatPayloadCodec
{
public:
    static constexpr std::uint8_t VERSION = 2;

    static constexpr std::size_t HEADER_SIZE = 18;

    static std::vector<std::uint8_t> encode(
        const ChatPayload& payload
    );

    static ChatPayload decode(
        const std::vector<std::uint8_t>& data
    );
};

#endif
