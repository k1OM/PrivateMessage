
#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#include "ChatController.hpp"
#include "MessageInputPanel.hpp"
#include "ConnectionPanel.hpp"
#include "StatusPanel.hpp"
#include "LogPanel.hpp"
#include "ChatPanel.hpp"

#include <wx/wx.h>
#include <wx/timer.h>

#include <memory>

class MainFrame : public wxFrame
{
private:
    ConnectionPanel* connectionPanel = nullptr;
    StatusPanel* statusPanel = nullptr;
    MessageInputPanel* messageInputPanel = nullptr;
    LogPanel* logPanel = nullptr;
    ChatPanel* chatPanel = nullptr;

    wxTimer eventTimer;

    std::unique_ptr<ChatController> controller;

    bool sessionActive;
    bool serverReady = false;

public:
    MainFrame();
    ~MainFrame() override;

private:
    void OnConnect();
    void OnDisconnect();
    void OnTimer(wxTimerEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnSendMessage();
    void AppendLog(const wxString& message);
    void UpdateButtons();
    void StopSession();
    void ProcessEvents();
};

#endif
