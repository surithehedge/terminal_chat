#include "protocol/frames.hpp"
#include <asio.hpp> // Using Asio for network byte order utilities

namespace tc::protocol {

// This function takes a JSON object and wraps it in our "TC" binary header
std::vector<uint8_t> serialize(const nlohmann::json& j) {
    std::string serial_json = j.dump();
    uint16_t payload_size = static_cast<uint16_t>(serial_json.size());

    // Prepare buffer: 4 bytes header + payload
    std::vector<uint8_t> buffer(4 + payload_size);

    // [0-1] Magic Bytes 'T' 'C'
    buffer[0] = 0x54;
    buffer[1] = 0x43;

    // [2-3] Payload Length in Big-Endian (Network Byte Order)
    // Section 6.1 requirement
    uint16_t net_len = htons(payload_size);
    std::memcpy(&buffer[2], &net_len, sizeof(uint16_t));

    // Payload
    std::memcpy(&buffer[4], serial_json.data(), payload_size);

    return buffer;
}

} // namespace tc::protocol
