
#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

// Declarations anticipees
class ClientSession;
class Message;

class SessionManager
{
public:
    using SessionId = std::uint64_t;

private:
    std::unordered_map<
        SessionId,
        std::shared_ptr<ClientSession>
    > sessions;

    SessionId nextSessionId = 1;

    static constexpr std::size_t MAX_SESSIONS = 100;

public:
    bool hasCapacity() const;

    SessionId add(std::shared_ptr<ClientSession> session);

    void remove(SessionId id);

    std::size_t count() const;

    void broadcastExcept(const std::shared_ptr<ClientSession>& sender, const Message& message);

    bool sendTo(SessionId recipientId, const Message& message);
};

#endif
