# No FetchContent for GTest needed!
add_executable(test_protocol test_protocol.cpp)

target_link_libraries(test_protocol
    PRIVATE
        tc_protocol
        tc_common
)

add_test(NAME ProtocolVerification COMMAND test_protocol)
add_executable(test_common test_common.cpp)
target_link_libraries(test_common PRIVATE tc_common nlohmann_json::nlohmann_json)
add_test(NAME CommonUtilities COMMAND test_common)
add_test(NAME ServerSmokeTest COMMAND tc_server)
add_test(NAME ClientSmokeTest COMMAND tc_client)
