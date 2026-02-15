add_executable(tc_server
    src/main.cpp
    src/core/server.cpp
    src/net/secure_session.cpp
    include/server/core/server.hpp
    include/server/net/secure_session.hpp
)

target_include_directories(tc_server PRIVATE include)
target_compile_options(tc_server PRIVATE ${TC_STRICT_FLAGS})

target_link_libraries(tc_server
    PRIVATE
        tc_common
        tc_protocol
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
)

if(WIN32)
    target_link_libraries(tc_server PRIVATE ws2_32)
endif()
