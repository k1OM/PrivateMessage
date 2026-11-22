#include "MainFrame.hpp"

#include <cstdint>
#include <string>
#include <vector>

void MainFrame::ProcessEvents()
{
    if (!controller)
    {
        return;
    }

    bool disconnected = false;

    std::vector<ChatEvent> events =
        controller->pollEvents();

    for (const ChatEvent& item : events)
    {
        if (item.type != ChatEventType::ServerError)
        {
            AppendLog(
                wxString::FromUTF8(item.text.c_str())
            );
        }

        switch (item.type)
        {
            case ChatEventType::Connected:
            {
                statusPanel->setStatus(ConnectionStatus::TcpConnected);

                break;
            }

            case ChatEventType::Ready:
            {
                serverReady = true;

                statusPanel->setStatus(ConnectionStatus::Connected);

                UpdateButtons();

                break;
            }

            case ChatEventType::Heartbeat:
            {
                break;
            }

            
            case ChatEventType::ChatMessage:
            {
                AppendLog(
                    "Message recu dans le salon"
                );

                break;
            }

            case ChatEventType::HeartbeatTimeout:
            {
                statusPanel->setStatus(ConnectionStatus::Timeout);
                break;
            }

            case ChatEventType::Disconnected:
            {
                disconnected = true;

                statusPanel->setStatus(ConnectionStatus::Disconnected);
                break;
            }


            case ChatEventType::SessionAssigned:
            {
                AppendLog("Identifiant attribue par le serveur");
                break;
            }

            case ChatEventType::PrivateMessage:
            {
                if (chatPanel)
                {
                    chatPanel->appendReceivedMessage(
                        item.senderId,
                        item.text
                    );
                }

                AppendLog(
                    "Message prive recu de la session " +
                    wxString::Format(
                        "%llu",
                        static_cast<unsigned long long>(item.senderId)
                    )
                );

                break;
            }

            case ChatEventType::ServerError:
            {
                AppendLog(
                    "Erreur serveur : " +
                    wxString::FromUTF8(item.text.c_str())
                );

                break;
            }

            case ChatEventType::Error:
            {
                statusPanel->setStatus(
                    ConnectionStatus::Error
                );

                break;
            }
        }
    }

    if (disconnected)
    {
        serverReady = false;
        sessionActive = false;

        statusPanel->setStatus(
            ConnectionStatus::Disconnected
        );

        UpdateButtons();
    }

}

void MainFrame::OnTimer(wxTimerEvent& event)
{
    if (!controller)
    {
        return;
    }

    ProcessEvents();

    if (!sessionActive)
    {
        StopSession();
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    // Arreter les mises a jour graphiques.
    eventTimer.Stop();

    // Fermer la connexion TCP et le heartbeat.
    StopSession();

    // Laisser wxWidgets detruire la fenetre.
    event.Skip();
}