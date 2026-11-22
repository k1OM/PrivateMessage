
#include "ChatPayloadCodec.hpp"
#include "Protocol.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include <string>


// Fonctions internes au fichier.

namespace
{
    constexpr std::size_t UINT64_SIZE = 8;

    void writeUint64(
        std::vector<std::uint8_t>& buffer,
        std::uint64_t value
    )
    {
        for (int i = 7; i >= 0; --i)
        {
            std::uint8_t byte =
                static_cast<std::uint8_t>(
                    (value >> (i * 8)) & 0xFF
                );

            buffer.push_back(byte);
        }
    }


    std::uint64_t readUint64(
        const std::vector<std::uint8_t>& buffer,
        std::size_t offset
    )
    {
        if (offset > buffer.size() ||
            buffer.size() - offset < UINT64_SIZE)
        {
            throw std::invalid_argument(
                "Lecture uint64 hors limites"
            );
        }

        std::uint64_t value = 0;

        for (std::size_t i = 0; i < UINT64_SIZE; ++i)
        {
            value =
                (value << 8) |
                static_cast<std::uint64_t>(
                    buffer[offset + i]
                );
        }

        return value;
    }


    bool isValidKind(ChatPayloadKind kind)
    {
        switch (kind)
        {
            case ChatPayloadKind::PrivateRequest:
            case ChatPayloadKind::PrivateDelivery:
            case ChatPayloadKind::PrivateAccepted:
            case ChatPayloadKind::PrivateRejected:
            {
                return true;
            }

            default:
            {
                return false;
            }
        }
    }


    void validateText(
        ChatPayloadKind kind,
        const std::string& text
    )
    {
        switch (kind)
        {
            case ChatPayloadKind::PrivateRequest:
            case ChatPayloadKind::PrivateDelivery:
            case ChatPayloadKind::PrivateRejected:
            {
                if (text.empty())
                {
                    throw std::invalid_argument(
                        "Texte obligatoire"
                    );
                }

                break;
            }

            case ChatPayloadKind::PrivateAccepted:
            {
                if (!text.empty())
                {
                    throw std::invalid_argument(
                        "PrivateAccepted doit avoir un texte vide"
                    );
                }

                break;
            }

            default:
            {
                throw std::invalid_argument(
                    "Nature de message invalide"
                );
            }
        }
    }
}


// Encodage du payload.

std::vector<std::uint8_t> ChatPayloadCodec::encode(
    const ChatPayload& payload
)
{
    // 1. Verification du type.

    if (!isValidKind(payload.kind))
    {
        throw std::invalid_argument(
            "Nature de message inconnue"
        );
    }

    // 2. Verification des identifiants.

    if (payload.sessionId == 0)
    {
        throw std::invalid_argument(
            "Identifiant de session invalide"
        );
    }

    if (payload.messageId == 0)
    {
        throw std::invalid_argument(
            "Identifiant de message invalide"
        );
    }

    // 3. Verification du texte.

    validateText(
        payload.kind,
        payload.text
    );

    // 4. Verification de la taille maximale.

    if (Protocol::MAX_PAYLOAD < HEADER_SIZE ||
        payload.text.size() >
            Protocol::MAX_PAYLOAD - HEADER_SIZE)
    {
        throw std::length_error(
            "Message trop grand"
        );
    }

    // 5. Construction du buffer.

    std::vector<std::uint8_t> result;

    result.reserve(
        HEADER_SIZE + payload.text.size()
    );

    result.push_back(VERSION);

    result.push_back(
        static_cast<std::uint8_t>(payload.kind)
    );

    // Offset 2 : sessionId sur 8 octets.

    writeUint64(
        result,
        payload.sessionId
    );

    // Offset 10 : messageId sur 8 octets.

    writeUint64(
        result,
        payload.messageId
    );

    // Offset 18 : texte.

    result.insert(
        result.end(),
        payload.text.begin(),
        payload.text.end()
    );

    return result;
}


// Decodage du payload.

ChatPayload ChatPayloadCodec::decode(
    const std::vector<std::uint8_t>& data
)
{
    // 1. Verification de la taille.

    if (data.size() < HEADER_SIZE)
    {
        throw std::invalid_argument(
            "Payload prive trop petit"
        );
    }

    if (data.size() > Protocol::MAX_PAYLOAD)
    {
        throw std::length_error(
            "Payload prive trop grand"
        );
    }

    // 2. Verification de la version.

    if (data[0] != VERSION)
    {
        throw std::invalid_argument(
            "Version non supportee"
        );
    }

    // 3. Lecture du type.

    ChatPayloadKind kind =
        static_cast<ChatPayloadKind>(data[1]);

    if (!isValidKind(kind))
    {
        throw std::invalid_argument(
            "Nature de message invalide"
        );
    }

    // 4. Lecture des identifiants.

    std::uint64_t sessionId =
        readUint64(data, 2);

    std::uint64_t messageId =
        readUint64(data, 10);

    if (sessionId == 0)
    {
        throw std::invalid_argument(
            "Identifiant de session invalide"
        );
    }

    if (messageId == 0)
    {
        throw std::invalid_argument(
            "Identifiant de message invalide"
        );
    }

    // 5. Lecture du texte.

    std::string text(
        data.begin() + HEADER_SIZE,
        data.end()
    );

    validateText(
        kind,
        text
    );

    // 6. Construction de l'objet.

    return ChatPayload{
        kind,
        sessionId,
        messageId,
        std::move(text)
    };
}
