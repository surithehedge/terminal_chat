add_library(tc_common STATIC
    src/logger.cpp
)

# Export include directory for other targets [cite: 45]
target_include_directories(tc_common PUBLIC include)

# Apply strict Master Spec flags only to our code
target_compile_options(tc_common PRIVATE ${TC_STRICT_FLAGS} ${TC_WERROR_FLAG})

# Linking rules: Static for internal components
target_link_libraries(tc_common PUBLIC Threads::Threads)
