add_executable(tc_server src/main.cpp)

target_include_directories(tc_server PRIVATE include)
target_compile_options(tc_server PRIVATE ${TC_STRICT_FLAGS})

target_link_libraries(tc_server
    PRIVATE
        tc_common
        tc_protocol
        Threads::Threads
)
