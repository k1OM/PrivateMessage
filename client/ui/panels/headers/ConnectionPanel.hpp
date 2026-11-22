
#ifndef CONNECTIONPANEL_HPP
#define CONNECTIONPANEL_HPP

#include <wx/wx.h>

#include <functional>

class ConnectionPanel : public wxPanel
{
private:
    wxButton* connectButton;
    wxButton* disconnectButton;

    std::function<void()> onConnect;
    std::function<void()> onDisconnect;

    void OnConnect(wxCommandEvent& event);
    void OnDisconnect(wxCommandEvent& event);

public:
    explicit ConnectionPanel(wxWindow* parent);

    void setOnConnect(
        std::function<void()> callback
    );

    void setOnDisconnect(
        std::function<void()> callback
    );

    void setConnected(bool connected);
};

#endif
