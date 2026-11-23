
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

    find_package(wxWidgets REQUIRED COMPONENTS core base)
    include(${wxWidgets_USE_FILE})

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
