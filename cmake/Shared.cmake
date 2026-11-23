
# ==================================================
# PrivateMessage - Bibliotheque commune
# ==================================================

add_library(privatechat_shared STATIC
    shared/protocol/Message.cpp
    shared/codec/MessageCodec.cpp
    shared/codec/ChatPayloadCodec.cpp
)

target_include_directories(privatechat_shared
    PUBLIC
        ${PROJECT_SOURCE_DIR}/shared/protocol/headers
        ${PROJECT_SOURCE_DIR}/shared/codec/headers
)
