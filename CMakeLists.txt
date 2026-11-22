
cmake_minimum_required(VERSION 3.20)

project(PrivateChat VERSION 1.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==================================================
# Options de compilation
# ==================================================

option(PRIVATECHAT_BUILD_CLIENT
       "Compiler les clients graphiques"
       ON)


# ==================================================
# Dependances communes
# ==================================================

find_path(ASIO_INCLUDE_DIR asio.hpp REQUIRED)
find_package(Threads REQUIRED)

include(CTest)

# ==================================================
# Dependances graphiques
# ==================================================

if(PRIVATECHAT_BUILD_CLIENT)

    find_package(wxWidgets REQUIRED COMPONENTS core base)
    include(${wxWidgets_USE_FILE})

endif()


# ==================================================
# Bibliotheque commune
# ==================================================

add_library(privatechat_shared STATIC
    shared/protocol/Message.cpp
    shared/codec/MessageCodec.cpp
    shared/codec/ChatPayloadCodec.cpp
)

target_include_directories(privatechat_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/shared/protocol
        ${CMAKE_CURRENT_SOURCE_DIR}/shared/codec
)

# ==================================================
# Bibliotheque Client
# ==================================================

if(PRIVATECHAT_BUILD_CLIENT)

add_executable(privatechat-client
    client/main.cpp
    client/Application.cpp
    client/NetworkClient.cpp

    # Interface graphique
    client/ui/MainFrame.cpp
    client/ui/MainFrameSession.cpp
    client/ui/MainFrameEvents.cpp
    client/ui/MainFrameMessages.cpp

    # Panneaux
    client/ui/panels/ChatPanel.cpp
    client/ui/panels/ConnectionPanel.cpp
    client/ui/panels/LogPanel.cpp
    client/ui/panels/MessageInputPanel.cpp
    client/ui/panels/StatusPanel.cpp

    # Controleur
    client/controller/ChatController.cpp
    client/controller/ChatControllerCallbacks.cpp
    client/controller/ChatControllerMessages.cpp

    # Handlers
    client/controller/handlers/ConnectionMessageHandler.cpp
    client/controller/handlers/PrivateMessageHandler.cpp
    client/controller/handlers/HeartbeatMessageHandler.cpp
)

target_include_directories(privatechat-client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/client/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/ui/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/ui/panels/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/events
        ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/handlers/headers
)

target_link_libraries(privatechat-client
    PRIVATE
        privatechat_network
        ${wxWidgets_LIBRARIES}
)



# ==================================================
# Bibliotheque Client 2
# ==================================================

add_executable(privatechat-client-2
    client/main.cpp
    client/Application.cpp
    client/NetworkClient.cpp

    # Interface graphique
    client/ui/MainFrame.cpp
    client/ui/MainFrameSession.cpp
    client/ui/MainFrameEvents.cpp
    client/ui/MainFrameMessages.cpp

    # Panneaux
    client/ui/panels/ChatPanel.cpp
    client/ui/panels/ConnectionPanel.cpp
    client/ui/panels/LogPanel.cpp
    client/ui/panels/MessageInputPanel.cpp
    client/ui/panels/StatusPanel.cpp

    # Controleur
    client/controller/ChatController.cpp
    client/controller/ChatControllerCallbacks.cpp
    client/controller/ChatControllerMessages.cpp

    # Handlers
    client/controller/handlers/ConnectionMessageHandler.cpp
    client/controller/handlers/PrivateMessageHandler.cpp
    client/controller/handlers/HeartbeatMessageHandler.cpp
)

target_include_directories(privatechat-client-2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/client/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/ui/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/ui/panels/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/events
        ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/handlers/headers
)

target_link_libraries(privatechat-client-2
    PRIVATE
        privatechat_network
        ${wxWidgets_LIBRARIES}
)

set(PRIVATECHAT_SERVER_IP "192.168.1.112"
    CACHE STRING "Adresse IP du serveur PrivateChat")

set(PRIVATECHAT_SERVER_PORT 5555
    CACHE STRING "Port du serveur PrivateChat")

target_compile_definitions(privatechat-client-2 PRIVATE
    PRIVATECHAT_SERVER_IP=\"${PRIVATECHAT_SERVER_IP}\"
    PRIVATECHAT_SERVER_PORT=${PRIVATECHAT_SERVER_PORT}
)



# ==================================================
# Bibliotheque reseau du client
# ==================================================

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
        ${CMAKE_CURRENT_SOURCE_DIR}/client/network/headers
        ${ASIO_INCLUDE_DIR}
)

target_link_libraries(privatechat_network
    PUBLIC
        privatechat_shared
        Threads::Threads
)

endif() # PRIVATECHAT_BUILD_CLIENT

# ==================================================
# Serveur de messagerie
# ==================================================

add_executable(privatechat-server
    server/main.cpp

    # Gestion du core
    server/core/ChatServer.cpp

    # Gestion des protocoles
    server/protocol/SessionMessageHandler.cpp

    # Gestion des sessions
    server/session/ClientSession.cpp
    server/session/SessionManager.cpp
    server/session/SessionInactivityMonitor.cpp

    #Gestionaire de reseau
    server/network/ClientSessionReader.cpp
    server/network/ClientSessionWriter.cpp
    server/network/ClientSessionConnection.cpp

)

target_include_directories(privatechat-server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/server/core/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/server/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/server/session/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/server/protocol/headers
        ${ASIO_INCLUDE_DIR}
)

target_link_libraries(privatechat-server
    PRIVATE
        privatechat_shared
        Threads::Threads
)

# ==================================================
# Tests unitaires
# ==================================================

if(BUILD_TESTING)

    # ----------------------------------------------
    # ChatPayloadCodec v2
    # ----------------------------------------------

    add_executable(test_chat_payload_v2
        tests/test_chat_payload_v2.cpp
    )

    target_link_libraries(test_chat_payload_v2
        PRIVATE
            privatechat_shared
    )

    add_test(
        NAME ChatPayloadV2
        COMMAND test_chat_payload_v2
    )

    # ----------------------------------------------
    # PrivateMessageHandler
    # ----------------------------------------------

    add_executable(test_private_message_handler
        tests/unit/test_private_message_handler.cpp
        client/controller/handlers/PrivateMessageHandler.cpp
    )

    target_include_directories(test_private_message_handler
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/handlers/headers
            ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/events
    )

    target_link_libraries(test_private_message_handler
        PRIVATE
            privatechat_shared
    )

    add_test(
        NAME PrivateMessageHandler
        COMMAND test_private_message_handler
    )

    # ----------------------------------------------
    # ConnectionMessageHandler
    # ----------------------------------------------

    add_executable(test_connection_message_handler
        tests/unit/test_connection_message_handler.cpp
        client/controller/handlers/ConnectionMessageHandler.cpp
    )

    target_include_directories(test_connection_message_handler
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/handlers/headers
            ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/events
    )

    target_link_libraries(test_connection_message_handler
        PRIVATE
            privatechat_shared
    )

    add_test(
        NAME ConnectionMessageHandler
        COMMAND test_connection_message_handler
    )

    # ----------------------------------------------
    # HeartbeatMessageHandler
    # ----------------------------------------------

    add_executable(test_heartbeat_message_handler
        tests/unit/test_heartbeat_message_handler.cpp
        client/controller/handlers/HeartbeatMessageHandler.cpp
    )

    target_include_directories(test_heartbeat_message_handler
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/handlers/headers
            ${CMAKE_CURRENT_SOURCE_DIR}/client/controller/events
    )

    target_link_libraries(test_heartbeat_message_handler
        PRIVATE
            privatechat_shared
    )

    add_test(
        NAME HeartbeatMessageHandler
        COMMAND test_heartbeat_message_handler
    )


    # ----------------------------------------------
    # Tests d'intégration du réseau client
    # ----------------------------------------------

    if(PRIVATECHAT_BUILD_CLIENT)

        # ----------------------------------------------
        # Integration : cycle PING / PONG
        # ----------------------------------------------

        add_executable(test_async_heartbeat
            tests/test_async_heartbeat.cpp
        )

        target_link_libraries(test_async_heartbeat
            PRIVATE
                privatechat_network
        )

        # ----------------------------------------------
        # Integration : timeout PONG
        # ----------------------------------------------

        add_executable(test_pong_timeout
            tests/test_pong_timeout.cpp
        )

        target_link_libraries(test_pong_timeout
            PRIVATE
                privatechat_network
        )

    endif()

    # ----------------------------------------------
    # Serveur silencieux pour les tests
    # ----------------------------------------------

    add_executable(test_silent_server
        tests/test_silent_server.cpp
    )

    target_include_directories(test_silent_server
        PRIVATE
            ${ASIO_INCLUDE_DIR}
    )

    target_link_libraries(test_silent_server
        PRIVATE
            privatechat_shared
            Threads::Threads
    )


    # ----------------------------------------------
    # Test time Out
    # ----------------------------------------------

    add_executable(test_session_inactivity_monitor
        tests/unit/test_session_inactivity_monitor.cpp
        server/session/SessionInactivityMonitor.cpp
    )

    target_include_directories(test_session_inactivity_monitor
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/server/session/headers
            ${ASIO_INCLUDE_DIR}
    )

    target_link_libraries(test_session_inactivity_monitor
        PRIVATE
            Threads::Threads
    )

    add_test(
        NAME SessionInactivityMonitor
        COMMAND test_session_inactivity_monitor
    )

    set_tests_properties(SessionInactivityMonitor PROPERTIES
        TIMEOUT 5
    )


    # ----------------------------------------------
    # Test client-inactive.cpp
    # ----------------------------------------------

    add_executable(test_inactive_client
        tests/test_inactive_client.cpp
    )

    target_link_libraries(test_inactive_client
        PRIVATE
            Threads::Threads
    )

    # ----------------------------------------------
    # Test session_manager_handler
    # ----------------------------------------------

    add_executable(test_session_message_handler
        tests/unit/test_session_message_handler.cpp
        server/protocol/SessionMessageHandler.cpp
    )

    target_include_directories(test_session_message_handler
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/server/headers
            ${CMAKE_CURRENT_SOURCE_DIR}/server/session/headers
            ${CMAKE_CURRENT_SOURCE_DIR}/server/protocol/headers
    )

    target_link_libraries(test_session_message_handler
        PRIVATE
            privatechat_shared
    )

    add_test(
        NAME SessionMessageHandler
        COMMAND test_session_message_handler
    )

    # ----------------------------------------------
    # Test test_tec-protocol.cpp
    # ----------------------------------------------

    add_executable(test_tcp_protocol
        tests/test_tcp_protocol.cpp
    )

    target_link_libraries(test_tcp_protocol
        PRIVATE
            privatechat_shared
            Threads::Threads
    )


endif()
