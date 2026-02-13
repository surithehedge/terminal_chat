cmake_minimum_required(VERSION 3.20)
project(terminal_chat VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 3.2 Compilation Policy [cite: 54, 55]
if(MSVC)
    add_compile_options(/W4 /WX /permissive- /std:c++20)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -fstack-protector-all)
endif()

# Dependency Management (FetchContent) [cite: 52]
include(FetchContent)

# Asio (Standalone)
FetchContent_Declare(asio URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-24-0.tar.gz)
FetchContent_MakeAvailable(asio)

# nlohmann_json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip)
FetchContent_MakeAvailable(json)

# FTXUI
FetchContent_Declare(ftxui URL https://github.com/ArthurSonzogni/FTXUI/archive/refs/tags/v3.0.0.tar.gz)
FetchContent_MakeAvailable(ftxui)

# OpenSSL is linked dynamically [cite: 56]
find_package(OpenSSL REQUIRED)

add_subdirectory(common)
add_subdirectory(protocol)
# server and client will be added in later phases
