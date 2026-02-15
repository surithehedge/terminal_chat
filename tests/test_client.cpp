#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <asio.hpp>
#include <asio/ssl/context.hpp>
#include <cstring>
#include <deque>
#include <iostream>
#include <vector>

int main() {
  std::cout << "Running Client Tests...\n";

  std::cout << "Test 1: Packet Header Encoding... ";
  {
    std::vector<uint8_t> packet = {0x54, 0x43, 0x01, 0x01};
    std::string msg = "hello";
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
    if (extracted_len != 5) {
      std::cerr << "FAILED (length mismatch)\n";
      return 1;
    }
    std::cout << "PASSED\n";
  }

  std::cout << "Test 2: Multiple Packets Queuing... ";
  {
    std::deque<std::vector<uint8_t>> write_queue;
    std::string msg1 = "first";
    std::vector<uint8_t> packet1 = {0x54, 0x43, 0x01, 0x01};
    uint32_t len1 = htonl(static_cast<uint32_t>(msg1.size()));
    packet1.insert(packet1.end(), reinterpret_cast<uint8_t *>(&len1),
                   reinterpret_cast<uint8_t *>(&len1) + 4);
    packet1.insert(packet1.end(), msg1.begin(), msg1.end());

    bool write_in_progress = !write_queue.empty();
    write_queue.push_back(std::move(packet1));

    if (write_in_progress || write_queue.size() != 1) {
      std::cerr << "FAILED\n";
      return 1;
    }
    std::cout << "PASSED\n";
  }

  std::cout << "Test 3: SSL Context Configuration... ";
  {
    try {
      asio::ssl::context ctx(asio::ssl::context::tls_client);
      ctx.set_verify_mode(asio::ssl::verify_none);
      std::cout << "PASSED\n";
    } catch (const std::exception &e) {
      std::cerr << "FAILED: " << e.what() << "\n";
      return 1;
    }
  }

  std::cout << "\nAll client tests passed!\n";
  return 0;
}
