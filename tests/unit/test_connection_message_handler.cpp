
#include "ConnectionMessageHandler.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

int main()
{
    ConnectionMessageHandler handler;

    // Test HELLO_ACK

    Message helloAck(
        MessageType::HelloAck,
        {}
    );

    ChatEvent ready =
        handler.handle(helloAck);

    assert(ready.type == ChatEventType::Ready);

    // Test SESSION_ASSIGNED : session 42

    std::vector<std::uint8_t> sessionPayload{
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x2A
    };

    Message assigned(
        MessageType::SessionAssigned,
        sessionPayload
    );

    ChatEvent sessionEvent =
        handler.handle(assigned);

    assert(
        sessionEvent.type ==
        ChatEventType::SessionAssigned
    );

    assert(sessionEvent.text == "Ma session : 42");

    std::cout
        << "ConnectionMessageHandler : tests OK"
        << std::endl;

    return 0;
}
