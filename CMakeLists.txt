cmake_minimum_required(VERSION 3.17)

project(cpp_engine LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

FetchContent_Declare(
    uwebsockets
    GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_SUBMODULES_RECURSE TRUE
    USES_TERMINAL_DOWNLOAD TRUE
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries/uWebSockets"
)

FetchContent_MakeAvailable(uwebsockets)

set(USOCKETS_DIR "${uwebsockets_SOURCE_DIR}/uSockets")

file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# uSockets sources: .c files, PLUS sni_tree.cpp (C++ file used by the SSL/crypto layer)
file(GLOB USOCKETS_SOURCES
    "${USOCKETS_DIR}/src/*.c"
    "${USOCKETS_DIR}/src/*/*.c"
    "${USOCKETS_DIR}/src/crypto/*.cpp"
)

add_executable(cpp_engine
    ${ENGINE_SOURCES}
    ${USOCKETS_SOURCES}
)

target_include_directories(cpp_engine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${uwebsockets_SOURCE_DIR}/src"
    "${USOCKETS_DIR}/src"
)

find_package(Threads REQUIRED)
target_link_libraries(cpp_engine PRIVATE Threads::Threads)

find_package(OpenSSL REQUIRED)
target_link_libraries(cpp_engine PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)
target_compile_definitions(cpp_engine PRIVATE
    LIBUS_USE_OPENSSL
)

# zlib — required for permessage-deflate (WebSocket compression)
find_package(ZLIB REQUIRED)
target_link_libraries(cpp_engine PRIVATE ZLIB::ZLIB)