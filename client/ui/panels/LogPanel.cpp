
#include "LogPanel.hpp"

LogPanel::LogPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY),
      logText(nullptr)
{
    wxBoxSizer* mainSizer =
        new wxBoxSizer(wxVERTICAL);

    logText = new wxTextCtrl(
        this,
        wxID_ANY,
        "",
        wxDefaultPosition,
        wxDefaultSize,
        wxTE_MULTILINE |
        wxTE_READONLY |
        wxTE_RICH2
    );

    mainSizer->Add(
        logText,
        1,
        wxEXPAND
    );

    SetSizer(mainSizer);
}


void LogPanel::append(const wxString& message)
{
    logText->AppendText(
        message + "\n"
    );
}


void LogPanel::clear()
{
    logText->Clear();
}
