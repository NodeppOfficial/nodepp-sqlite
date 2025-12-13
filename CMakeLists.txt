cmake_minimum_required(VERSION 3.0.0)
project(nodepp-sqlite VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(NODEPP_SQLITE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Search for Nodepp Core
if (NOT TARGET nodepp)
    find_package(nodepp REQUIRED)
    if(NOT nodepp_FOUND)
        message(FATAL_ERROR "Nodepp not found. Please install nodepp.")
    endif()
endif()

# Search for SQLite3 Library File
find_library(SQLITE3_LIBRARY NAMES sqlite3 REQUIRED)
if(NOT SQLITE3_LIBRARY)
    message(FATAL_ERROR "SQLite3 library file not found. Please install libsqlite3-dev or equivalent files.")
endif()

# Search for SQLite3 Headers
find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h REQUIRED
    HINTS /usr/include /usr/local/include
)
if(NOT SQLITE3_INCLUDE_DIR)
    message(FATAL_ERROR "SQLite3 header file (sqlite3.h) not found.")
endif()

# interface-target
add_library(nodepp-sqlite INTERFACE)

# Combine all necessary include directories
target_include_directories(nodepp-sqlite INTERFACE
    ${NODEPP_SQLITE_INCLUDE_DIR}
    ${SQLITE3_INCLUDE_DIR}
)

# Link against all dependencies
target_link_libraries(nodepp-sqlite INTERFACE
    nodepp
    ${SQLITE3_LIBRARY}
)