add_library(tc_common STATIC
    src/logger.cpp
    include/common/logger.hpp
)

target_include_directories(tc_common PUBLIC include)

target_compile_options(tc_common PRIVATE ${TC_STRICT_FLAGS})

target_link_libraries(tc_common PUBLIC Threads::Threads)
