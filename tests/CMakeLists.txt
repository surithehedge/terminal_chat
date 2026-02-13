add_executable(test_common test_common.cpp)
target_link_libraries(test_common PRIVATE tc_common)
target_compile_options(test_common PRIVATE ${TC_STRICT_FLAGS} ${TC_WERROR_FLAG})

# Register with CTest
add_test(NAME CommonUtilsTest COMMAND test_common)
