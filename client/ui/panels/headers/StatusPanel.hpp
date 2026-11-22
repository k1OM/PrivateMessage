
#ifndef STATUSPANEL_HPP
#define STATUSPANEL_HPP

#include <wx/wx.h>

enum class ConnectionStatus
{
    Offline,
    Connecting,
    TcpConnected,
    Connected,
    Disconnected,
    Timeout,
    Error
};

class StatusPanel : public wxPanel
{
private:
    wxStaticText* statusLabel;

public:
    explicit StatusPanel(wxWindow* parent);

    void setStatus(ConnectionStatus status);
};

#endif
