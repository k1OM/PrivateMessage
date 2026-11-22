#pragma once

#include "Protocol.hpp"

namespace ClientConfig
{
    #ifdef PRIVATECHAT_SERVER_IP
        inline constexpr const char* SERVER_IP = PRIVATECHAT_SERVER_IP;
    #else
        inline constexpr const char* SERVER_IP = "127.0.0.1";
    #endif

    #ifdef PRIVATECHAT_SERVER_PORT
        inline constexpr unsigned short SERVER_PORT = PRIVATECHAT_SERVER_PORT;
    #else
        inline constexpr unsigned short SERVER_PORT = Protocol::DEFAULT_PORT;
    #endif
}