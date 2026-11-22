#include "MessageInputPanel.hpp"

#include <utility>

MessageInputPanel::MessageInputPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY),
      recipientInput(nullptr),
      messageInput(nullptr),
      sendButton(nullptr)
{
    wxBoxSizer* mainSizer =
        new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* recipientSizer =
        new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* messageSizer =
        new wxBoxSizer(wxHORIZONTAL);

    // Destinataire

    wxStaticText* recipientLabel =
        new wxStaticText(
            this,
            wxID_ANY,
            "Destinataire (ID) :"
        );

    recipientInput = new wxTextCtrl(
        this,
        wxID_ANY,
        ""
    );

    recipientSizer->Add(
        recipientLabel,
        0,
        wxALIGN_CENTER_VERTICAL | wxRIGHT,
        8
    );

    recipientSizer->Add(
        recipientInput,
        1
    );

    // Message

    messageInput = new wxTextCtrl(
        this,
        wxID_ANY,
        "",
        wxDefaultPosition,
        wxDefaultSize,
        wxTE_PROCESS_ENTER
    );

    sendButton = new wxButton(
        this,
        wxID_ANY,
        "Envoyer"
    );

    messageSizer->Add(
        messageInput,
        1,
        wxRIGHT,
        8
    );

    messageSizer->Add(
        sendButton,
        0
    );

    // Organisation

    mainSizer->Add(
        recipientSizer,
        0,
        wxEXPAND | wxBOTTOM,
        8
    );

    mainSizer->Add(
        messageSizer,
        0,
        wxEXPAND
    );

    SetSizer(mainSizer);

    // Evenements

    sendButton->Bind(
        wxEVT_BUTTON,
        &MessageInputPanel::OnSend,
        this
    );

    messageInput->Bind(
        wxEVT_TEXT_ENTER,
        &MessageInputPanel::OnSend,
        this
    );

    setSendingEnabled(false);
}


bool MessageInputPanel::getRecipientId(
    std::uint64_t& id
) const
{
    wxString value =
        recipientInput->GetValue();

    value.Trim(true);
    value.Trim(false);

    wxULongLong_t parsedId = 0;

    if (!value.ToULongLong(&parsedId) ||
        parsedId == 0)
    {
        return false;
    }

    id = static_cast<std::uint64_t>(
        parsedId
    );

    return true;
}


std::string MessageInputPanel::getMessageText() const
{
    wxString value =
        messageInput->GetValue();

    wxScopedCharBuffer utf8 =
        value.ToUTF8();

    if (!utf8.data())
    {
        return {};
    }

    return std::string(
        utf8.data(),
        utf8.length()
    );
}


void MessageInputPanel::clearMessage()
{
    messageInput->Clear();
    messageInput->SetFocus();
}


void MessageInputPanel::setSendingEnabled(bool enabled)
{
    recipientInput->Enable(enabled);
    messageInput->Enable(enabled);
    sendButton->Enable(enabled);
}


void MessageInputPanel::setOnSend(
    std::function<void()> callback
)
{
    onSend = std::move(callback);
}

void MessageInputPanel::OnSend(wxCommandEvent& event)
{
    if (onSend)
    {
        onSend();
    }
}
