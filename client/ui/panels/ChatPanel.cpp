
#include "ChatPanel.hpp"

ChatPanel::ChatPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY),
      conversationText(nullptr)
{
    wxBoxSizer* mainSizer =
        new wxBoxSizer(wxVERTICAL);

    wxStaticText* title =
        new wxStaticText(
            this,
            wxID_ANY,
            "Conversation"
        );

    conversationText =
        new wxTextCtrl(
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
        title,
        0,
        wxBOTTOM,
        6
    );

    mainSizer->Add(
        conversationText,
        1,
        wxEXPAND
    );

    SetSizer(mainSizer);
}



void ChatPanel::appendReceivedMessage(
    std::uint64_t senderId,
    const std::string& message
)
{
    wxString header = wxString::Format(
        "Session %llu : ",
        static_cast<unsigned long long>(senderId)
    );

    wxString text =
        wxString::FromUTF8(
            message.c_str()
        );

    conversationText->AppendText(
        header + text + "\n"
    );
}



void ChatPanel::appendSentMessage(
    std::uint64_t recipientId,
    const std::string& message
)
{
    wxString text =
        wxString::FromUTF8(
            message.c_str()
        );

    wxString header = wxString::Format(
        "Moi -> Session %llu : ",
        static_cast<unsigned long long>(recipientId)
    );

    conversationText->AppendText(
        header + text + "\n"
    );
}


void ChatPanel::clear()
{
    conversationText->Clear();
}
