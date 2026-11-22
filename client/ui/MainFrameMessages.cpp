#include "MainFrame.hpp"

#include <cstdint>
#include <exception>
#include <string>

void MainFrame::OnSendMessage()
{
    // 1. Verifier la connexion

    if (!controller || !sessionActive || !serverReady)
    {
        AppendLog(
            "Impossible d'envoyer : serveur non pret"
        );

        return;
    }

    // 2. Verifier le panneau graphique

    if (!messageInputPanel)
    {
        AppendLog(
            "Erreur : panneau de saisie indisponible"
        );

        return;
    }

    // 3. Recuperer le destinataire

    std::uint64_t recipientId = 0;

    if (!messageInputPanel->getRecipientId(recipientId))
    {
        AppendLog(
            "Erreur : identifiant de destinataire invalide"
        );

        return;
    }

    // 4. Recuperer le message

    std::string text = messageInputPanel->getMessageText();

    if (text.find_first_not_of(" \t\r\n") == std::string::npos)
    {
        AppendLog(
            "Erreur : message vide"
        );

        return;
    }

    // 5. Transmettre au controleur

    try
    {
        controller->sendPrivateMessage(recipientId, text);

        if (chatPanel)
        {
            chatPanel->appendSentMessage(
                recipientId,
                text
            );
        }

        AppendLog(
            "Demande d'envoi vers session " +
            wxString::Format(
                "%llu",
                static_cast<unsigned long long>(recipientId)
            )
        );

        // Effacer uniquement le champ message.
        messageInputPanel->clearMessage();
    }
    catch (const std::exception& error)
    {
        AppendLog(
            "Erreur d'envoi : " +
            wxString::FromUTF8(error.what())
        );
    }
}