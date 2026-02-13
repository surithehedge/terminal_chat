#include <iostream>
#include <vector>
#include "protocol/frames.hpp"
#include "common/logger.hpp"

int main() {
    using namespace tc::protocol;

    nlohmann::json test_data = {{"msg", "hello"}};
    std::string json_str = test_data.dump();

    auto buffer = serialize(0x01, test_data);

    if (buffer.size() < 8) {
        std::cerr << "Test Failed: Header size less than 8 bytes\n";
        return 1;
    }

    if (buffer[0] != 0x54 || buffer[1] != 0x43) {
        std::cerr << "Test Failed: Magic Number mismatch\n";
        return 1;
    }

    if (buffer[2] != 0x01) {
        std::cerr << "Test Failed: Version mismatch\n";
        return 1;
    }

    if (buffer[7] != static_cast<uint8_t>(json_str.size())) {
        std::cerr << "Test Failed: Length encoding error\n";
        return 1;
    }

    std::cout << "Protocol test passed successfully!\n";
    return 0;
}
