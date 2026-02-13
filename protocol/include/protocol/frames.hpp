#pragma once
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>

namespace tc::protocol {

struct MsgHeader {
    uint8_t magic[2] = {0x54, 0x43};
    uint8_t version = 0x01;
    uint8_t type = 0x01;
    uint32_t payload_length = 0;
};

std::vector<uint8_t> serialize(uint8_t type, const nlohmann::json& j);

}
