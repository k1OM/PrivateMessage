
# ==================================================
# PrivateMessage - Serveur
# ==================================================

# Bibliotheque des composants serveur

add_library(privatechat_server_core STATIC
    server/protocol/SessionMessageHandler.cpp

    server/session/ClientSession.cpp
    server/session/SessionManager.cpp
    server/session/SessionInactivityMonitor.cpp

    server/network/ClientSessionReader.cpp
    server/network/ClientSessionWriter.cpp
    server/network/ClientSessionConnection.cpp
)

target_include_directories(privatechat_server_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/server/session/headers
        ${PROJECT_SOURCE_DIR}/server/protocol/headers
        ${ASIO_INCLUDE_DIR}
)

target_link_libraries(privatechat_server_core
    PUBLIC
        privatechat_shared
        Threads::Threads
)

# Executable serveur

add_executable(privatechat-server
    server/main.cpp
    server/core/ChatServer.cpp
)

target_include_directories(privatechat-server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/server/core/headers
)

target_link_libraries(privatechat-server
    PRIVATE
        privatechat_server_core
)
