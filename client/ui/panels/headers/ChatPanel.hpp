
#ifndef CHATPANEL_HPP
#define CHATPANEL_HPP

#include <wx/wx.h>

#include <cstdint>
#include <string>

class ChatPanel : public wxPanel
{
private:
    wxTextCtrl* conversationText;

public:
    explicit ChatPanel(wxWindow* parent);

    void appendReceivedMessage(
        std::uint64_t senderId,
        const std::string& message
    );

    void appendSentMessage(
        std::uint64_t recipientId,
        const std::string& message
    );

    void clear();
};

#endif
