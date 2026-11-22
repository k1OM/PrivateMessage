
#ifndef MESSAGEINPUTPANEL_HPP
#define MESSAGEINPUTPANEL_HPP

#include <wx/wx.h>

#include <cstdint>
#include <functional>
#include <string>

class MessageInputPanel : public wxPanel
{
private:
    wxTextCtrl* recipientInput;
    wxTextCtrl* messageInput;
    wxButton* sendButton;

    std::function<void()> onSend;

    void OnSend(wxCommandEvent& event);

public:
    explicit MessageInputPanel(wxWindow* parent);

    bool getRecipientId(std::uint64_t& id) const;

    std::string getMessageText() const;

    void clearMessage();

    void setSendingEnabled(bool enabled);

    void setOnSend(std::function<void()> callback);
};

#endif
