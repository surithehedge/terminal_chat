cmake_minimum_required(VERSION 3.20)
project(terminal_chat VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(TC_STRICT_FLAGS /W4 /permissive- /std:c++20)
else()
    set(TC_STRICT_FLAGS -Wall -Wextra -Wpedantic -fstack-protector-all)
endif()

include(FetchContent)

FetchContent_Declare(asio URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-24-0.tar.gz)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include)
include_directories(${ASIO_INCLUDE_DIR})

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip
)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json SYSTEM INTERFACE ${json_SOURCE_DIR}/include)
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
endif()

set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_ENABLE_STRICT_COMPILE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

find_package(OpenSSL REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
add_subdirectory(common)
add_subdirectory(protocol)
add_subdirectory(server)
add_subdirectory(client)

enable_testing()
add_subdirectory(tests)
