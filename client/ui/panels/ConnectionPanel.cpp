
#include "ConnectionPanel.hpp"

#include <utility>

ConnectionPanel::ConnectionPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY),
      connectButton(nullptr),
      disconnectButton(nullptr)
{
    // Organisation horizontale

    wxBoxSizer* buttonSizer =
        new wxBoxSizer(wxHORIZONTAL);

    // Creation des boutons

    connectButton = new wxButton(
        this,
        wxID_ANY,
        "Connecter"
    );

    disconnectButton = new wxButton(
        this,
        wxID_ANY,
        "Deconnecter"
    );

    buttonSizer->Add(
        connectButton,
        1,
        wxRIGHT,
        8
    );

    buttonSizer->Add(
        disconnectButton,
        1
    );

    SetSizer(buttonSizer);

    // Evenements wxWidgets

    connectButton->Bind(
        wxEVT_BUTTON,
        &ConnectionPanel::OnConnect,
        this
    );

    disconnectButton->Bind(
        wxEVT_BUTTON,
        &ConnectionPanel::OnDisconnect,
        this
    );

    setConnected(false);
}


void ConnectionPanel::setOnConnect(
    std::function<void()> callback
)
{
    onConnect = std::move(callback);
}


void ConnectionPanel::setOnDisconnect(
    std::function<void()> callback
)
{
    onDisconnect = std::move(callback);
}


void ConnectionPanel::setConnected(bool connected)
{
    connectButton->Enable(!connected);
    disconnectButton->Enable(connected);
}


void ConnectionPanel::OnConnect(
    wxCommandEvent& event
)
{
    if (onConnect)
    {
        onConnect();
    }
}


void ConnectionPanel::OnDisconnect(
    wxCommandEvent& event
)
{
    if (onDisconnect)
    {
        onDisconnect();
    }
}
