
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
    ON
)

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

    if(MINGW)

        set(PRIVATECHAT_WX_CONFIG
            "/usr/local/x86_64-w64-mingw32/bin/wx-config"
            CACHE FILEPATH "wx-config pour Windows"
        )

        execute_process(
            COMMAND "${PRIVATECHAT_WX_CONFIG}" --cxxflags
            OUTPUT_VARIABLE PRIVATECHAT_WX_CXXFLAGS
            OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY
        )

        execute_process(
            COMMAND "${PRIVATECHAT_WX_CONFIG}" --libs core,base
            OUTPUT_VARIABLE PRIVATECHAT_WX_LIBFLAGS
            OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY
        )

        separate_arguments(
            PRIVATECHAT_WX_CXXFLAGS
            UNIX_COMMAND
            "${PRIVATECHAT_WX_CXXFLAGS}"
        )

        separate_arguments(
            PRIVATECHAT_WX_LIBFLAGS
            UNIX_COMMAND
            "${PRIVATECHAT_WX_LIBFLAGS}"
        )

        add_library(privatechat_wx INTERFACE)

        target_compile_options(privatechat_wx
            INTERFACE
                ${PRIVATECHAT_WX_CXXFLAGS}
        )

        target_link_libraries(privatechat_wx
            INTERFACE
                ${PRIVATECHAT_WX_LIBFLAGS}
        )

        set(wxWidgets_LIBRARIES privatechat_wx)

    else()

        find_package(wxWidgets REQUIRED COMPONENTS core base)
        include(${wxWidgets_USE_FILE})

    endif()

endif()


# ==================================================
# Bibliotheque commune (toujours disponible)
# ==================================================

include(cmake/Shared.cmake)

# ==================================================
# Clients (optionnels)
# ==================================================

include(cmake/Client.cmake)

# ==================================================
# Serveur (toujours disponible)
# ==================================================

include(cmake/Server.cmake)

# ==================================================
# Tests
# ==================================================

include(cmake/Tests.cmake)
