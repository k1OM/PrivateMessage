
#ifndef LOGPANEL_HPP
#define LOGPANEL_HPP

#include <wx/wx.h>

class LogPanel : public wxPanel
{
private:
    wxTextCtrl* logText;

public:
    explicit LogPanel(wxWindow* parent);

    void append(const wxString& message);

    void clear();
};

#endif
