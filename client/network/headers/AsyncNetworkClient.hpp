
#ifndef ASYNCNETWORKCLIENT_HPP
#define ASYNCNETWORKCLIENT_HPP

#include <asio.hpp>

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Message.hpp"
#include "MessageCodec.hpp"

class AsyncNetworkClient :
    public std::enable_shared_from_this<AsyncNetworkClient>
{
public:
    using ConnectCallback = std::function<void()>;

    using MessageCallback = std::function<void(const Message&)>;

    using DisconnectCallback = std::function<void(const std::string&)>;

private:

    enum class ConnectionState
    {
        Idle,
        Resolving,
        Connecting,
        Connected,
        Stopped
    };

    asio::ip::tcp::resolver resolver;
    asio::ip::tcp::socket socket;

    MessageCodec::Header header;

    std::vector<std::uint8_t> payload;
    std::deque<std::vector<std::uint8_t>> outgoingMessages;

    bool connected;
    bool stopped;

    ConnectionState connectionState;
    ConnectCallback onConnect;
    MessageCallback onMessage;
    DisconnectCallback onDisconnect;

public:
    explicit AsyncNetworkClient(asio::io_context& context);

    void connect(
        const std::string& address,
        std::uint16_t port
    );

    void sendMessage(const Message& message);

    void disconnect();

    bool isConnected() const;

    void setOnConnect(ConnectCallback callback);
    void setOnMessage(MessageCallback callback);
    void setOnDisconnect(DisconnectCallback callback);

private:
    void readHeader();
    void readPayload(std::uint32_t length);
    void writeNext();

    void handleMessage(const Message& message);

    void closeConnection(const std::string& reason);
};

#endif
