# ==================================================
# Tests unitaires
# ==================================================

if(BUILD_TESTING)

    # ----------------------------------------------
    # ChatPayloadCodec v2
    # ----------------------------------------------

    add_executable(test_chat_payload_v2
        tests/unit/test_chat_payload_v2.cpp
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
    # Tests unitaires : Message
    # ----------------------------------------------

    add_executable(test_message
        tests/unit/test_message.cpp
    )

    target_link_libraries(test_message
        PRIVATE
            privatechat_shared
    )

    target_compile_options(test_message
        PRIVATE
            -UNDEBUG
    )

    add_test(
        NAME Message
        COMMAND test_message
    )

    # ----------------------------------------------
    # Tests unitaires : MessageCodec
    # ----------------------------------------------

    add_executable(test_message_codec
        tests/unit/test_message_codec.cpp
    )

    target_link_libraries(test_message_codec
        PRIVATE
            privatechat_shared
    )

    add_test(
        NAME MessageCodec
        COMMAND test_message_codec
    )

    # ----------------------------------------------
    # Tests unitaires : ChatPayloadCodec
    # ----------------------------------------------

    add_executable(test_chat_payload_codec
        tests/unit/test_chat_payload_codec.cpp
    )

    target_link_libraries(test_chat_payload_codec
        PRIVATE
            privatechat_shared
    )

    target_compile_options(test_chat_payload_codec
        PRIVATE
            -UNDEBUG
    )

    add_test(
        NAME ChatPayloadCodec
        COMMAND test_chat_payload_codec
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
            tests/integration/client/test_async_heartbeat.cpp
        )

        target_link_libraries(test_async_heartbeat
            PRIVATE
                privatechat_network
        )

        # ----------------------------------------------
        # Integration : timeout PONG
        # ----------------------------------------------

        add_executable(test_pong_timeout
            tests/integration/client/test_pong_timeout.cpp
        )

        target_link_libraries(test_pong_timeout
            PRIVATE
                privatechat_network
        )

        # ----------------------------------------------
        # Integration : timeout PONG automatise
        # ----------------------------------------------

        find_package(Python3 REQUIRED COMPONENTS Interpreter)

        add_test(
            NAME PongTimeoutIntegration
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/run_pong_timeout.py
                ${CMAKE_CURRENT_BINARY_DIR}
        )

        set_tests_properties(PongTimeoutIntegration PROPERTIES
            TIMEOUT 30
            RUN_SERIAL TRUE
        )

        # ----------------------------------------------
        # Integration : heartbeat asynchrone automatise
        # ----------------------------------------------

        add_test(
            NAME AsyncHeartbeatIntegration
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/run_async_heartbeat.py
                ${CMAKE_CURRENT_BINARY_DIR}
        )

        set_tests_properties(AsyncHeartbeatIntegration PROPERTIES
            TIMEOUT 45
            RUN_SERIAL TRUE
        )


        # ----------------------------------------------
        # MultiClient integration test
        # ----------------------------------------------

        add_executable(test_multiple_clients
            tests/integration/server/test_multiple_clients.cpp
            tests/integration/helpers/MultiClientTest.cpp

            client/NetworkClient.cpp
        )

        target_include_directories(test_multiple_clients
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/helpers/headers
                ${CMAKE_CURRENT_SOURCE_DIR}/client/headers
        )

        target_link_libraries(test_multiple_clients
            PRIVATE
                privatechat_shared
                privatechat_network
        )

        # ----------------------------------------------
        # MultiClient integration python
        # ----------------------------------------------


        add_test(
            NAME MultipleClientsIntegration
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/run_multiple_clients.py
                $<TARGET_FILE:privatechat-server>
                $<TARGET_FILE:test_multiple_clients>
        )

        set_tests_properties(MultipleClientsIntegration
            PROPERTIES
                TIMEOUT 35
                RUN_SERIAL TRUE
        )

        # ----------------------------------------------
        # MultiClient session deconnection
        # ----------------------------------------------


        add_executable(test_session_disconnect
            tests/integration/server/test_session_disconnect.cpp
            client/NetworkClient.cpp
        )

        target_include_directories(test_session_disconnect
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/client/headers
        )

        target_link_libraries(test_session_disconnect
            PRIVATE
                privatechat_shared
                privatechat_network
        )


        # ----------------------------------------------
        # Session disconnect integration test python
        # ----------------------------------------------

        add_test(
            NAME SessionDisconnectIntegration
            COMMAND
                ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/run_session_disconnect.py
                $<TARGET_FILE:privatechat-server>
                $<TARGET_FILE:test_session_disconnect>
        )

        set_tests_properties(SessionDisconnectIntegration
            PROPERTIES
                TIMEOUT 35
                RUN_SERIAL TRUE
        )

    endif()

    # ----------------------------------------------
    # Serveur silencieux pour les tests
    # ----------------------------------------------

    add_executable(test_silent_server
        tests/integration/helpers/test_silent_server.cpp
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
        tests/integration/server/test_inactive_client.cpp
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
        tests/integration/server/test_tcp_protocol.cpp
    )

    target_link_libraries(test_tcp_protocol
        PRIVATE
            privatechat_shared
            Threads::Threads
    )


    # ----------------------------------------------
    # SessionManager Unit Test
    # ----------------------------------------------


    add_executable(test_session_manager
        tests/unit/test_session_manager.cpp
    )

    target_link_libraries(test_session_manager
        PRIVATE
            privatechat_server_core
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_session_manager
            PRIVATE
                -UNDEBUG
        )
    endif()

    add_test(
        NAME SessionManager
        COMMAND test_session_manager
    )

    # ==================================================
    # ConnectionConfig - Test unitaire
    # ==================================================

    add_executable(test_connection_config
        tests/unit/test_connection_config.cpp
    )

    target_include_directories(test_connection_config
        PRIVATE
            ${PROJECT_SOURCE_DIR}/client/config/headers
    )

    # Conserver les assertions actives en Debug et Release
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_connection_config
            PRIVATE
                -UNDEBUG
        )
    endif()

    add_test(
        NAME ConnectionConfig
        COMMAND test_connection_config
    )



    # ==================================================
    # ConfigLoader - Test unitaire
    # ==================================================

    add_executable(test_config_loader
        tests/unit/test_config_loader.cpp
        client/config/ConfigLoader.cpp
    )

    target_include_directories(test_config_loader
        PRIVATE
            ${PROJECT_SOURCE_DIR}/client/config/headers
    )

    target_compile_definitions(test_config_loader
        PRIVATE
            PROJECT_SOURCE_DIR_PATH="${PROJECT_SOURCE_DIR}"
    )

    # Conserver les assertions en Release
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_config_loader
            PRIVATE -UNDEBUG
        )
    endif()

    add_test(
        NAME ConfigLoader
        COMMAND test_config_loader
    )

    set_tests_properties(ConfigLoader PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # ==================================================
    # Test de resolution DNS et connexion TCP asynchrone
    # ==================================================

    add_executable(test_async_dns
        tests/integration/client/test_async_dns.cpp
    )

    target_include_directories(test_async_dns
        PRIVATE
            ${PROJECT_SOURCE_DIR}/client/network/headers
    )

    target_link_libraries(test_async_dns
        PRIVATE
            privatechat_network
            Threads::Threads
    )

    add_test(
        NAME AsyncDnsIntegration
        COMMAND test_async_dns
    )

    set_tests_properties(AsyncDnsIntegration PROPERTIES
        TIMEOUT 10
    )

    # ==================================================    
    # Test du cycle de vie multithread de ChatController
    # ==================================================
    add_executable(test_chat_controller_lifecycle
        tests/integration/client/test_chat_controller_lifecycle.cpp

        client/controller/ChatController.cpp
        client/controller/ChatControllerCallbacks.cpp
        client/controller/ChatControllerMessages.cpp

        client/controller/handlers/ConnectionMessageHandler.cpp
        client/controller/handlers/PrivateMessageHandler.cpp
        client/controller/handlers/HeartbeatMessageHandler.cpp
    )

    target_include_directories(test_chat_controller_lifecycle
        PRIVATE
            ${PROJECT_SOURCE_DIR}/client/controller/headers
            ${PROJECT_SOURCE_DIR}/client/controller/events
            ${PROJECT_SOURCE_DIR}/client/controller/handlers/headers
            ${PROJECT_SOURCE_DIR}/client/network/headers
    )

    target_link_libraries(test_chat_controller_lifecycle
        PRIVATE
            privatechat_network
            privatechat_shared
            Threads::Threads
    )

    add_test(
        NAME ChatControllerLifecycle
        COMMAND test_chat_controller_lifecycle
    )

    set_tests_properties(ChatControllerLifecycle PROPERTIES
        TIMEOUT 15
    )

endif()
