#pragma once

#include "asio/steady_timer.hpp"
#include "protocol/frames.hpp"
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

namespace tc::net {

class SecureSession : public std::enable_shared_from_this<SecureSession> {
public:
  SecureSession(asio::ip::tcp::socket socket, asio::ssl::context &ctx);
  void start();

private:
  void do_handshake();
  void do_read_header();
  void do_read_body(uint32_t length);
  void check_timeout();
  void stop();
  void write(const std::vector<uint8_t> &packet);
  void do_write();

  asio::strand<asio::any_io_executor> strand_;
  asio::ssl::stream<asio::ip::tcp::socket> ssl_socket_;
  asio::steady_timer timer_;
  std::deque<std::vector<uint8_t>> write_queue_;
  uint8_t header_buffer_[8];
};
} // namespace tc::net
