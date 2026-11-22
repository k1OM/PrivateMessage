
#include "SessionManager.hpp"
#include "ClientSession.hpp"
#include "Message.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

bool SessionManager::hasCapacity() const
{
    return sessions.size() < MAX_SESSIONS;
}

SessionManager::SessionId SessionManager::add(
    std::shared_ptr<ClientSession> session
)
{
    if (!session)
    {
        throw std::invalid_argument("Session nulle");
    }

    if (!hasCapacity())
    {
        throw std::runtime_error(
            "Limite de sessions atteinte"
        );
    }

    if (nextSessionId ==
        std::numeric_limits<SessionId>::max())
    {
        throw std::overflow_error(
            "Identifiants de session epuises"
        );
    }

    SessionId id = nextSessionId++;

    sessions.emplace(id, std::move(session));

    return id;
}


void SessionManager::broadcastExcept(
    const std::shared_ptr<ClientSession>& sender,
    const Message& message
)
{
    for (const auto& entry : sessions)
    {
        const std::shared_ptr<ClientSession>& recipient =
            entry.second;

        if (!recipient)
        {
            continue;
        }

        if (recipient == sender)
        {
            continue;
        }

        recipient->sendMessage(message);
    }
}


bool SessionManager::sendTo(
    SessionId recipientId,
    const Message& message
)
{
    auto iterator = sessions.find(recipientId);

    if (iterator == sessions.end())
    {
        return false;
    }

    const std::shared_ptr<ClientSession>& recipient =
        iterator->second;

    if (!recipient)
    {
        return false;
    }

    recipient->sendMessage(message);

    return true;
}

void SessionManager::remove(SessionId id)
{
    sessions.erase(id);
}

std::size_t SessionManager::count() const
{
    return sessions.size();
}
