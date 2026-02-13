add_library(tc_protocol STATIC
    src/framing.cpp
)

target_include_directories(tc_protocol PUBLIC include)

# Apply strict flags
target_compile_options(tc_protocol PRIVATE ${TC_STRICT_FLAGS} ${TC_WERROR_FLAG})

# Link internal utilities and external JSON
target_link_libraries(tc_protocol
    PUBLIC
        tc_common
        nlohmann_json::nlohmann_json
)
