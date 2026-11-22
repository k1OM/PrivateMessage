
#include "HeartbeatMessageHandler.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

int main()
{
    HeartbeatMessageHandler handler;

    // Test 1 : message PONG valide.

    Message pong(
        MessageType::Pong,
        {}
    );

    ChatEvent event =
        handler.handle(pong);

    assert(
        event.type ==
        ChatEventType::Heartbeat
    );

    assert(event.text == "PONG recu");

    // Test 2 : message incorrect.

    Message hello(
        MessageType::Hello,
        {}
    );

    bool rejected = false;

    try
    {
        handler.handle(hello);
    }
    catch (const std::invalid_argument&)
    {
        rejected = true;
    }

    assert(rejected);

    std::cout
        << "HeartbeatMessageHandler : tests OK"
        << std::endl;

    return 0;
}
