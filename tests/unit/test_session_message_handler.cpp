
#include "SessionMessageHandler.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

bool testHello()
{
    SessionMessageHandler handler;

    std::vector<Message> sentMessages;
    bool closeRequested = false;

    handler.setSessionId(0x0102030405060708ULL);

    handler.setOnSend(
        [&sentMessages](const Message& message)
        {
            sentMessages.push_back(message);
        }
    );

    handler.setOnClose(
        [&closeRequested]()
        {
            closeRequested = true;
        }
    );

    handler.processMessage(
        Message(MessageType::Hello, {})
    );

    if (closeRequested || sentMessages.size() != 2)
    {
        return false;
    }

    if (sentMessages[0].getType() != MessageType::HelloAck)
    {
        return false;
    }

    if (sentMessages[1].getType() != MessageType::SessionAssigned)
    {
        return false;
    }

    // Verifie l'encodage big-endian de l'identifiant.
    std::vector<std::uint8_t> expectedId =
    {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    };

    return sentMessages[1].getPayload() == expectedId;
}

bool testInvalidHello()
{
    SessionMessageHandler handler;

    bool closeRequested = false;
    int sentCount = 0;

    handler.setSessionId(0);

    handler.setOnClose(
        [&closeRequested]()
        {
            closeRequested = true;
        }
    );

    handler.setOnSend(
        [&sentCount](const Message&)
        {
            ++sentCount;
        }
    );

    handler.processMessage(
        Message(MessageType::Hello, {})
    );

    return closeRequested && sentCount == 0;
}


bool testPing()
{
    SessionMessageHandler handler;

    int pongCount = 0;

    handler.setSessionId(42);

    handler.setOnSend(
        [&pongCount](const Message& message)
        {
            if (message.getType() == MessageType::Pong)
            {
                ++pongCount;
            }
        }
    );

    // Initialiser correctement la session.

    handler.processMessage(
        Message(MessageType::Hello, {})
    );

    // Verifier que la session est active.

    if (handler.getState() != SessionProtocolState::Active)
    {
        return false;
    }

    // Maintenant, PING est autorise.

    handler.processMessage(
        Message(MessageType::Ping, {})
    );

    return pongCount == 1
        && handler.getState() == SessionProtocolState::Active;
}


bool testChat()
{
    SessionMessageHandler handler;

    int chatCount = 0;
    int privateCount = 0;

    handler.setOnChat(
        [&chatCount](const Message&)
        {
            ++chatCount;
        }
    );

    handler.setOnPrivateChat(
        [&privateCount](const Message&)
        {
            ++privateCount;
        }
    );

    handler.setSessionId(42);

    handler.processMessage(
        Message(MessageType::Hello, {})
    );

    if (handler.getState() != SessionProtocolState::Active)
    {
        return false;
    }

    handler.processMessage(
        Message(MessageType::Chat, {})
    );

    handler.processMessage(
        Message(MessageType::PrivateChat, {})
    );

    return chatCount == 1 && privateCount == 1 && handler.getState() == SessionProtocolState::Active;
}


bool testHelloTransition()
{
    SessionMessageHandler handler;

    int sentCount = 0;
    bool closeRequested = false;

    // Un nouveau gestionnaire attend HELLO.

    if (handler.getState() !=
        SessionProtocolState::AwaitingHello)
    {
        return false;
    }

    handler.setSessionId(42);

    handler.setOnSend(
        [&sentCount](const Message&)
        {
            ++sentCount;
        }
    );

    handler.setOnClose(
        [&closeRequested]()
        {
            closeRequested = true;
        }
    );

    // Reception du premier HELLO.

    handler.processMessage(
        Message(MessageType::Hello, {})
    );

    // La session doit devenir active.

    if (handler.getState() !=
        SessionProtocolState::Active)
    {
        return false;
    }

    // HELLO_ACK + SESSION_ASSIGNED.

    if (sentCount != 2)
    {
        return false;
    }

    return !closeRequested;
}



bool testPingBeforeHello()
{
    SessionMessageHandler handler;

    bool closeRequested = false;
    int sentCount = 0;

    handler.setSessionId(42);

    handler.setOnClose(
        [&closeRequested]()
        {
            closeRequested = true;
        }
    );

    handler.setOnSend(
        [&sentCount](const Message&)
        {
            ++sentCount;
        }
    );

    // Verifier l'etat initial.

    if (handler.getState() !=
        SessionProtocolState::AwaitingHello)
    {
        return false;
    }

    // ATTENTION :
    // Aucun HELLO ne doit etre envoye ici.

    handler.processMessage(
        Message(MessageType::Ping, {})
    );

    std::cout
        << "[DEBUG] closeRequested = "
        << closeRequested
        << std::endl;

    std::cout
        << "[DEBUG] sentCount = "
        << sentCount
        << std::endl;

    std::cout
        << "[DEBUG] state = "
        << static_cast<int>(handler.getState())
        << std::endl;

    return closeRequested
        && sentCount == 0
        && handler.getState() ==
            SessionProtocolState::Closing;
}



bool testDoubleHello()
{
    SessionMessageHandler handler;

    int sentCount = 0;
    int closeCount = 0;

    handler.setSessionId(42);

    handler.setOnSend(
        [&sentCount](const Message&)
        {
            ++sentCount;
        }
    );

    handler.setOnClose(
        [&closeCount]()
        {
            ++closeCount;
        }
    );

    // Premier HELLO : autorise.

    handler.processMessage(
        Message(MessageType::Hello, {})
    );

    if (handler.getState() != SessionProtocolState::Active)
    {
        return false;
    }

    if (sentCount != 2 || closeCount != 0)
    {
        return false;
    }

    // Deuxieme HELLO : interdit.

    handler.processMessage(
        Message(MessageType::Hello, {})
    );

    if (handler.getState() != SessionProtocolState::Closing)
    {
        return false;
    }

    // Aucun nouveau message ne doit etre envoye.

    return sentCount == 2 && closeCount == 1;
}


bool testChatBeforeHello()
{
    const std::vector<MessageType> forbiddenTypes =
    {
        MessageType::Chat,
        MessageType::PrivateChat
    };

    for (MessageType type : forbiddenTypes)
    {
        SessionMessageHandler handler;

        int closeCount = 0;
        int sendCount = 0;
        int chatCount = 0;
        int privateCount = 0;

        handler.setSessionId(42);

        handler.setOnClose(
            [&closeCount]()
            {
                ++closeCount;
            }
        );

        handler.setOnSend(
            [&sendCount](const Message&)
            {
                ++sendCount;
            }
        );

        handler.setOnChat(
            [&chatCount](const Message&)
            {
                ++chatCount;
            }
        );

        handler.setOnPrivateChat(
            [&privateCount](const Message&)
            {
                ++privateCount;
            }
        );

        // Aucun HELLO n'est envoye.

        handler.processMessage(
            Message(type, {})
        );

        if (handler.getState() !=
            SessionProtocolState::Closing)
        {
            return false;
        }

        if (closeCount != 1 || sendCount != 0)
        {
            return false;
        }

        if (chatCount != 0 || privateCount != 0)
        {
            return false;
        }

        // Une session fermee doit ignorer
        // tous les messages suivants.

        handler.processMessage(
            Message(MessageType::Hello, {})
        );

        handler.processMessage(
            Message(MessageType::Ping, {})
        );

        handler.processMessage(
            Message(MessageType::PrivateChat, {})
        );

        if (closeCount != 1 ||
            sendCount != 0 ||
            chatCount != 0 ||
            privateCount != 0)
        {
            return false;
        }
    }

    return true;
}



int main()
{
    if (!testHello())
    {
        std::cerr << "ECHEC : HELLO" << std::endl;
        return 1;
    }

    std::cout << "HELLO : OK" << std::endl;

    if (!testInvalidHello())
    {
        std::cerr << "ECHEC : HELLO invalide" << std::endl;
        return 1;
    }

    std::cout << "HELLO invalide : OK" << std::endl;

    if (!testPing())
    {
        std::cerr << "ECHEC : PING" << std::endl;
        return 1;
    }

    std::cout << "PING : OK" << std::endl;

    if (!testChat())
    {
        std::cerr << "ECHEC : CHAT / PRIVATE_CHAT" << std::endl;
        return 1;
    }

    std::cout << "CHAT / PRIVATE_CHAT : OK" << std::endl;

    if (!testPingBeforeHello())
    {
        std::cerr << "ECHEC : PING avant HELLO" << std::endl;

        return 1;
    }

    std::cout << "PING avant HELLO : OK" << std::endl;

    if (!testDoubleHello())
    {
        std::cerr << "ECHEC : double HELLO" << std::endl;

        return 1;
    }

    std::cout << "Double HELLO : OK" << std::endl; 

    if (!testChatBeforeHello())
    {
        std::cerr
            << "ECHEC : CHAT avant HELLO"
            << std::endl;

        return 1;
    }

    std::cout
        << "CHAT avant HELLO : OK"
        << std::endl;




    return 0;
}
