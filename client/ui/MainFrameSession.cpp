
#include "MainFrame.hpp"
#include "Protocol.hpp"
#include "ClientConfig.hpp"

#include <exception>
#include <memory>
#include <string>

void MainFrame::OnConnect()
{
    if (sessionActive)
    {
        return;
    }

    try
    {
        controller = std::make_unique<ChatController>();

        controller->start(
            ClientConfig::SERVER_IP,
            ClientConfig::SERVER_PORT
        );

        sessionActive = true;

        statusPanel->setStatus(ConnectionStatus::Connecting);

        AppendLog("Connexion demandee");

        serverReady = false;
        UpdateButtons();
    }
    catch (const std::exception& error)
    {
        AppendLog(
            "Erreur connexion : " +
            wxString::FromUTF8(error.what())
        );

        StopSession();
    }
}

void MainFrame::StopSession()
{
    if (controller)
    {
        controller->stop();

        // Afficher les evenements restants
        // avant de detruire le controleur.
        ProcessEvents();

        controller.reset();
    }

    sessionActive = false;
    serverReady = false;

    if (statusPanel)
    {
        statusPanel->setStatus(
            ConnectionStatus::Offline
        );
    }

    UpdateButtons();
}

void MainFrame::OnDisconnect()
{
    if (!sessionActive)
    {
        return;
    }

    AppendLog("Deconnexion demandee");

    StopSession();
}