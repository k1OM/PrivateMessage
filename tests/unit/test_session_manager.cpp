
#include "SessionManager.hpp"
#include "ClientSession.hpp"

#include <asio.hpp>

#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>
#include <stdexcept>
#include <vector>

int main()
{
    asio::io_context ioContext;

    SessionManager manager;

    // 1. Le gestionnaire doit etre vide.
    assert(manager.count() == 0);
    assert(manager.hasCapacity());

    // 2. Creer deux sessions sans connexion TCP.
    asio::ip::tcp::socket socket1(ioContext);
    asio::ip::tcp::socket socket2(ioContext);

    std::shared_ptr<ClientSession> session1 =
        std::make_shared<ClientSession>(std::move(socket1));

    std::shared_ptr<ClientSession> session2 =
        std::make_shared<ClientSession>(std::move(socket2));

    // 3. Ajouter les deux sessions.
    std::uint64_t id1 = manager.add(session1);
    std::uint64_t id2 = manager.add(session2);

    assert(id1 != 0);
    assert(id2 != 0);
    assert(id1 != id2);

    assert(manager.count() == 2);

    // 4. Supprimer la premiere session.
    manager.remove(id1);

    assert(manager.count() == 1);

    // 5. Supprimer la deuxieme session.
    manager.remove(id2);

    assert(manager.count() == 0);



    // 6. La capacite doit toujours etre disponible.
    assert(manager.hasCapacity());



    // 7. Supprimer une session inexistante.
    manager.remove(9999);

    assert(manager.count() == 0);

    // 8. Ajouter une nouvelle session.
    asio::ip::tcp::socket socket3(ioContext);

    std::shared_ptr<ClientSession> session3 =
        std::make_shared<ClientSession>(std::move(socket3));

    std::uint64_t id3 = manager.add(session3);

    // L'identifiant ne doit pas etre reutilise.
    assert(id3 != id1);
    assert(id3 != id2);

    assert(manager.count() == 1);

    // 9. Nettoyer la derniere session.
    manager.remove(id3);

    assert(manager.count() == 0);



    // 10. Tester la capacite maximale du gestionnaire.
    constexpr std::size_t MAX_TEST_SESSIONS = 100;

    std::vector<std::uint64_t> sessionIds;

    sessionIds.reserve(MAX_TEST_SESSIONS);

    for (std::size_t i = 0; i < MAX_TEST_SESSIONS; ++i)
    {
        asio::ip::tcp::socket socket(ioContext);

        std::shared_ptr<ClientSession> session =
            std::make_shared<ClientSession>(std::move(socket));

        std::uint64_t id = manager.add(session);

        sessionIds.push_back(id);
    }

    assert(manager.count() == MAX_TEST_SESSIONS);
    assert(!manager.hasCapacity());

    // 11. Verifier que la 101e session est refusee.
    bool capacityRejected = false;

    try
    {
        asio::ip::tcp::socket socket(ioContext);

        std::shared_ptr<ClientSession> session =
            std::make_shared<ClientSession>(std::move(socket));

        manager.add(session);
    }
    catch (const std::runtime_error&)
    {
        capacityRejected = true;
    }

    assert(capacityRejected);
    assert(manager.count() == MAX_TEST_SESSIONS);

    // 12. Supprimer une session et retrouver de la capacite.
    manager.remove(sessionIds.front());

    assert(manager.count() == MAX_TEST_SESSIONS - 1);
    assert(manager.hasCapacity());

    // 13. Ajouter une nouvelle session.
    asio::ip::tcp::socket replacementSocket(ioContext);

    std::shared_ptr<ClientSession> replacement =
        std::make_shared<ClientSession>(
            std::move(replacementSocket)
        );

    std::uint64_t replacementId = manager.add(replacement);

    assert(replacementId != sessionIds.front());
    assert(manager.count() == MAX_TEST_SESSIONS);
    assert(!manager.hasCapacity());

    // 14. Nettoyage.
    for (std::uint64_t id : sessionIds)
    {
        manager.remove(id);
    }

    manager.remove(replacementId);

    assert(manager.count() == 0);
    assert(manager.hasCapacity());



    std::cout
        << "SessionManager : test reussi"
        << std::endl;

    return 0;
}
