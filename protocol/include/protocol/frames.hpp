#pragma once
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>

namespace tc::protocol {

struct MsgHeader {
    uint8_t magic[2] = {0x54, 0x43};
    uint16_t payload_length;
};

// Added this declaration:
std::vector<uint8_t> serialize(const nlohmann::json& j);

} // namespace tc::protocol
