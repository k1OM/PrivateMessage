
#include "MultiClientTest.hpp"
#include "Protocol.hpp"

#include <iostream>

int main()
{
    constexpr std::size_t CLIENT_COUNT = 5;

    MultiClientTest test(
        "127.0.0.1",
        Protocol::DEFAULT_PORT,
        CLIENT_COUNT
    );

    if (!test.run())
    {
        std::cerr
            << "Test multiclient echoue"
            << std::endl;

        return 1;
    }

    std::cout
        << "Tous les clients ont termine avec succes !"
        << std::endl;

    return 0;
}
