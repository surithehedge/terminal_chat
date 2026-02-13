cmake_minimum_required(VERSION 3.20)
project(terminal_chat VERSION 1.0.0 LANGUAGES CXX)

# Standard Compliance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define target-specific strict flags variable
if(MSVC)
    set(TC_STRICT_FLAGS /W4 /permissive- /std:c++20)
else()
    set(TC_STRICT_FLAGS -Wall -Wextra -Wpedantic -fstack-protector-all)
endif()

include(FetchContent)

# 1. Asio (Standalone)
FetchContent_Declare(asio URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-24-0.tar.gz)
FetchContent_MakeAvailable(asio)

# 2. nlohmann_json (Header-only target)
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip
)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    # This creates a "virtual" library that carries the include path
    add_library(nlohmann_json INTERFACE)
    # json_SOURCE_DIR points to build/_deps/json-src/
    target_include_directories(nlohmann_json SYSTEM INTERFACE ${json_SOURCE_DIR}/include)
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
endif()

# 3. FTXUI (Disable examples to prevent build errors)
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_ENABLE_STRICT_COMPILE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(ftxui URL https://github.com/ArthurSonzogni/FTXUI/archive/refs/tags/v3.0.0.tar.gz)
FetchContent_MakeAvailable(ftxui)

# 4. OpenSSL (Dynamic Linking)
find_package(OpenSSL REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Discover the system's threading library
find_package(Threads REQUIRED)
# Subdirectories
add_subdirectory(common)
add_subdirectory(protocol)

enable_testing()
add_subdirectory(tests)
