#pragma once
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <deque>
#include <memory>
#include <string>
#include <vector>

namespace tc::net {

class SecureClient : public std::enable_shared_from_this<SecureClient> {
public:
  SecureClient(asio::io_context &io_ctx, asio::ssl::context &ssl_ctx);
  void connect(const std::string &host, const std::string &port);
  void send(const std::string &message);

private:
  void do_connect(asio::ip::tcp::resolver::results_type endpoints);
  void do_handshake();

  void do_read_header();
  void do_read_body(uint32_t length);

  void do_write();

  asio::io_context &io_ctx_;
  asio::ip::tcp::resolver resolver_;
  asio::ssl::stream<asio::ip::tcp::socket> ssl_socket_;

  uint8_t header_buffer_[8];
  std::deque<std::vector<uint8_t>> write_queue_;

  std::vector<uint8_t> read_accumulator_;
  uint8_t temp_buffer_[4096];
};

} // namespace tc::net
