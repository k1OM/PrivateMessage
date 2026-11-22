
#include "StatusPanel.hpp"

StatusPanel::StatusPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY),
      statusLabel(nullptr)
{
    wxBoxSizer* mainSizer =
        new wxBoxSizer(wxHORIZONTAL);

    statusLabel = new wxStaticText(
        this,
        wxID_ANY,
        "Etat : hors ligne"
    );

    mainSizer->Add(
        statusLabel,
        1,
        wxEXPAND
    );

    SetSizer(mainSizer);
}


void StatusPanel::setStatus(ConnectionStatus status)
{
    switch (status)
    {
        case ConnectionStatus::Offline:
        {
            statusLabel->SetLabel("Etat : hors ligne");
            break;
        }

        case ConnectionStatus::Connecting:
        {
            statusLabel->SetLabel("Etat : connexion...");
            break;
        }

        case ConnectionStatus::TcpConnected:
        {
            statusLabel->SetLabel("Etat : TCP connecte");
            break;
        }

        case ConnectionStatus::Connected:
        {
            statusLabel->SetLabel("Etat : connecte au serveur");
            break;
        }

        case ConnectionStatus::Disconnected:
        {
            statusLabel->SetLabel("Etat : deconnecte");
            break;
        }

        case ConnectionStatus::Timeout:
        {
            statusLabel->SetLabel("Etat : timeout heartbeat");
            break;
        }

        case ConnectionStatus::Error:
        {
            statusLabel->SetLabel("Etat : erreur");
            break;
        }
    }

    Layout();
}
