add_library(tc_protocol STATIC
    src/framing.cpp
    include/protocol/frames.hpp
)

target_include_directories(tc_protocol PUBLIC include)

target_compile_options(tc_protocol PRIVATE ${TC_STRICT_FLAGS})

target_link_libraries(tc_protocol
    PUBLIC
        tc_common
        nlohmann_json::nlohmann_json
)
