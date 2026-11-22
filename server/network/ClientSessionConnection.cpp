#include "ClientSession.hpp"

#include <iostream>

void ClientSession::closeConnection()
{
    if (closed)
    {
        return;
    }

    closed = true;

    asio::error_code error;

    // Annulation du timer
    if (inactivityMonitor)
	{
	    inactivityMonitor->stop();
	}

    // Annulation des operations reseau
    socket.cancel(error);

    // Arret de la communication TCP
    socket.shutdown(
        asio::ip::tcp::socket::shutdown_both,
        error
    );

    // Fermeture du socket
    socket.close(error);

    //outgoingMessages.clear();

    std::cout << "Client deconnecte" << std::endl;

    if (onClose)
    {
        onClose();
    }
}