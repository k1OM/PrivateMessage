
#pragma once

#include "ConnectionConfig.hpp"

#include <string>

class ConfigLoader
{
public:

    static ConnectionConfig loadFromFile(
        const std::string& path
    );
};
