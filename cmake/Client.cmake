
# ==================================================
# PrivateMessage - Clients
# ==================================================

if(PRIVATECHAT_BUILD_CLIENT)

    # ----------------------------------------------
    # Bibliotheque reseau asynchrone
    # ----------------------------------------------

    add_library(privatechat_network STATIC
        client/network/AsyncNetworkClient.cpp
        client/network/AsyncNetworkConnection.cpp
        client/network/AsyncNetworkReader.cpp
        client/network/AsyncNetworkWriter.cpp

        client/network/AsyncHeartbeatManager.cpp
        client/network/AsyncHeartbeatTimers.cpp
        client/network/AsyncHeartbeatProtocol.cpp
    )

    target_include_directories(privatechat_network
        PUBLIC
            ${PROJECT_SOURCE_DIR}/client/network/headers
            ${ASIO_INCLUDE_DIR}
    )

    target_link_libraries(privatechat_network
        PUBLIC
            privatechat_shared
            Threads::Threads
    )

    # ----------------------------------------------
    # Sources communes aux deux clients graphiques
    # ----------------------------------------------

    set(PRIVATECHAT_CLIENT_SOURCES
        client/main.cpp
        client/Application.cpp
        client/NetworkClient.cpp

        client/ui/MainFrame.cpp
        client/ui/MainFrameSession.cpp
        client/ui/MainFrameEvents.cpp
        client/ui/MainFrameMessages.cpp

        client/ui/panels/ChatPanel.cpp
        client/ui/panels/ConnectionPanel.cpp
        client/ui/panels/LogPanel.cpp
        client/ui/panels/MessageInputPanel.cpp
        client/ui/panels/StatusPanel.cpp

        client/controller/ChatController.cpp
        client/controller/ChatControllerCallbacks.cpp
        client/controller/ChatControllerMessages.cpp

        client/controller/handlers/ConnectionMessageHandler.cpp
        client/controller/handlers/PrivateMessageHandler.cpp
        client/controller/handlers/HeartbeatMessageHandler.cpp

        client/config/ConfigLoader.cpp
    )

    # ----------------------------------------------
    # Fonction de creation d'un client graphique
    # ----------------------------------------------

    function(privatechat_add_client TARGET_NAME)

        add_executable(${TARGET_NAME}
            ${PRIVATECHAT_CLIENT_SOURCES}
        )

        target_include_directories(${TARGET_NAME}
            PRIVATE
                ${PROJECT_SOURCE_DIR}/client/headers
                ${PROJECT_SOURCE_DIR}/client/ui/headers
                ${PROJECT_SOURCE_DIR}/client/ui/panels/headers
                ${PROJECT_SOURCE_DIR}/client/controller/headers
                ${PROJECT_SOURCE_DIR}/client/controller/events
                ${PROJECT_SOURCE_DIR}/client/controller/handlers/headers

                # Configuration JSON
                ${PROJECT_SOURCE_DIR}/client/config/headers
        )

        target_link_libraries(${TARGET_NAME}
            PRIVATE
                privatechat_network
                ${wxWidgets_LIBRARIES}
        )

    endfunction()

    # ----------------------------------------------
    # Premier client
    # ----------------------------------------------

    privatechat_add_client(privatechat-client)

    # ----------------------------------------------
    # Deuxieme client
    # ----------------------------------------------

    privatechat_add_client(privatechat-client-2)

    set(PRIVATECHAT_SERVER_IP "192.168.1.112"
        CACHE STRING "Adresse IP du serveur PrivateChat"
    )

    set(PRIVATECHAT_SERVER_PORT 5555
        CACHE STRING "Port du serveur PrivateChat"
    )

    target_compile_definitions(privatechat-client-2
        PRIVATE
            PRIVATECHAT_SERVER_IP=\"${PRIVATECHAT_SERVER_IP}\"
            PRIVATECHAT_SERVER_PORT=${PRIVATECHAT_SERVER_PORT}
    )

endif()
