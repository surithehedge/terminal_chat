add_library(tc_common STATIC
    src/logger.cpp
)

target_include_directories(tc_common PUBLIC include)

# Apply 3.2 Compilation Policy & 3.3 Sanitizers [cite: 54, 60]
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(tc_common PRIVATE -Wall -Wextra -Wpedantic -Werror -fstack-protector-all)

    # Enable ASan and UBSan in Debug builds on Linux [cite: 60]
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(tc_common PRIVATE -fsanitize=address -fsanitize=undefined)
        target_link_options(tc_common PRIVATE -fsanitize=address -fsanitize=undefined)
    endif()
elseif(MSVC)
    target_compile_options(tc_common PRIVATE /W4 /WX /permissive-) # [cite: 55]
endif()
