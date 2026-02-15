add_executable(tc_client
    src/main.cpp
    src/net/secure_client.cpp
    include/client/net/secure_client.hpp
)

target_include_directories(tc_client PRIVATE include)
target_compile_options(tc_client PRIVATE ${TC_STRICT_FLAGS})

target_link_libraries(tc_client
    PRIVATE
        tc_common
        tc_protocol
        ftxui::component ftxui::dom ftxui::screen
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
)
