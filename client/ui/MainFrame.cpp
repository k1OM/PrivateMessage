#include "MainFrame.hpp"

MainFrame::MainFrame()
    : wxFrame(
          nullptr,
          wxID_ANY,
          "PrivateChat",
          wxDefaultPosition,
          wxSize(800, 650)
      ),
      eventTimer(this),
      controller(nullptr),
      sessionActive(false)
{
    // 1. Panneau principal

    wxPanel* panel = new wxPanel(this);

    // 2. Organisation graphique

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 3. Titre

    wxStaticText* title =
        new wxStaticText(
            panel,
            wxID_ANY,
            "PrivateChat - Version 1.3"
        );

    wxFont titleFont = title->GetFont();

    titleFont.SetPointSize(16);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);

    title->SetFont(titleFont);

    // 4. Etat de connexion

    statusPanel = new StatusPanel(panel);

    // 5. Journal

    logPanel = new LogPanel(panel);
    chatPanel = new ChatPanel(panel);

    // 6. Notre classe enfant

    messageInputPanel = new MessageInputPanel(panel);

    messageInputPanel->setOnSend(
        [this]()
        {
            OnSendMessage();
        }
    );

    // Connection
    connectionPanel = new ConnectionPanel(panel);

    connectionPanel->setOnConnect(
        [this]()
        {
            OnConnect();
        }
    );

    connectionPanel->setOnDisconnect(
        [this]()
        {
            OnDisconnect();
        }
    );

    // Assemblage graphique

    mainSizer->Add(
        title,
        0,
        wxALL,
        12
    );

    mainSizer->Add(
        statusPanel,
        0,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        12
    );

    // Zone de conversation principale

    mainSizer->Add(
        chatPanel,
        2,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        12
    );

    // Journal technique

    mainSizer->Add(
        logPanel,
        1,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        12
    );

    // Saisie et envoi

    mainSizer->Add(
        messageInputPanel,
        0,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        12
    );

    // Connexion et deconnexion

    mainSizer->Add(
        connectionPanel,
        0,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        12
    );

    panel->SetSizer(mainSizer);


    // 10. Initialisation

     Bind(
        wxEVT_TIMER,
        &MainFrame::OnTimer,
        this,
        eventTimer.GetId()
    );

    // Fermeture de la fenetre

    Bind(
        wxEVT_CLOSE_WINDOW,
        &MainFrame::OnClose,
        this
    );

    // Initialisation de l'interface

    UpdateButtons();

    AppendLog("Application demarree");
    AppendLog("Serveur : 127.0.0.1:5555");

    // Interroger le controleur toutes les 100 ms

    eventTimer.Start(100);

    Centre();
}

MainFrame::~MainFrame()
{
    eventTimer.Stop();
    StopSession();
}

void MainFrame::AppendLog(const wxString& message)
{
    if (logPanel)
    {
        logPanel->append(message);
    }
}

void MainFrame::UpdateButtons()
{
    if (connectionPanel)
    {
        connectionPanel->setConnected(sessionActive);
    }

    if (messageInputPanel)
    {
        messageInputPanel->setSendingEnabled(
            sessionActive && serverReady
        );
    }
}