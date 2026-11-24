
#pragma once

#include <cstdint>
#include <string>

class ConnectionConfig
{
public:

    std::string host;
    std::uint16_t port = 0;

    bool isValid() const
    {
        if (host.empty())
        {
            return false;
        }

        if (port == 0)
        {
            return false;
        }

        return true;
    }
};
