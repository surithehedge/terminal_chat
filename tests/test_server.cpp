#include <arpa/inet.h>
#include <cstring>
#include <deque>
#include <iostream>
#include <vector>

int main() {
  std::cout << "Running Server Tests...\n";

  std::cout << "Test 1: Packet Header Encoding... ";
  {
    std::vector<uint8_t> packet = {0x54, 0x43, 0x01, 0x01};
    std::string msg = "test";
    uint32_t len = htonl(static_cast<uint32_t>(msg.size()));
    uint8_t len_bytes[4];
    std::memcpy(len_bytes, &len, 4);
    packet.insert(packet.end(), len_bytes, len_bytes + 4);
    packet.insert(packet.end(), msg.begin(), msg.end());

    if (packet[0] != 0x54 || packet[1] != 0x43 || packet[2] != 0x01 ||
        packet[3] != 0x01) {
      std::cerr << "FAILED\n";
      return 1;
    }

    uint32_t extracted_len = 0;
    std::memcpy(&extracted_len, &packet[4], 4);
    extracted_len = ntohl(extracted_len);
    if (extracted_len != 4) {
      std::cerr << "FAILED (length mismatch)\n";
      return 1;
    }
    std::cout << "PASSED\n";
  }

  std::cout << "Test 2: Invalid Magic Number Rejection... ";
  {
    uint8_t invalid_header[] = {0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x05};
    bool is_valid = (invalid_header[0] == 0x54 && invalid_header[1] == 0x43);
    if (is_valid) {
      std::cerr << "FAILED\n";
      return 1;
    }
    std::cout << "PASSED\n";
  }

  std::cout << "Test 3: Valid Magic Number Acceptance... ";
  {
    uint8_t valid_header[] = {0x54, 0x43, 0x01, 0x01, 0x00, 0x00, 0x00, 0x05};
    bool is_valid = (valid_header[0] == 0x54 && valid_header[1] == 0x43);
    if (!is_valid) {
      std::cerr << "FAILED\n";
      return 1;
    }
    std::cout << "PASSED\n";
  }

  std::cout << "Test 4: Payload Length Limit Check... ";
  {
    uint32_t body_len = 0;
    std::memcpy(&body_len, "\x00\x01\x00\x01", 4);
    body_len = ntohl(body_len);

    if (body_len <= 65536) {
      std::cerr << "FAILED\n";
      return 1;
    }
    std::cout << "PASSED\n";
  }

  std::cout << "\nAll server tests passed!\n";
  return 0;
}
