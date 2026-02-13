#include "protocol/frames.hpp"
#include <asio.hpp>

namespace tc::protocol {

std::vector<uint8_t> serialize(uint8_t type, const nlohmann::json& j) {
    std::string serialized_json = j.dump();
    uint32_t len = static_cast<uint32_t>(serialized_json.size());

    MsgHeader header;
    header.type = type;
    header.payload_length = htonl(len);

    std::vector<uint8_t> buffer;
    buffer.reserve(sizeof(MsgHeader) + len);

    uint8_t* header_ptr = reinterpret_cast<uint8_t*>(&header);
    buffer.insert(buffer.end(), header_ptr, header_ptr + sizeof(MsgHeader));

    buffer.insert(buffer.end(), serialized_json.begin(), serialized_json.end());

    return buffer;
}

}
