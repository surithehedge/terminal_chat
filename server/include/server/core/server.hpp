#pragma once
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ssl/context.hpp"
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <cstdint>
#include <cstring>
#include <memory>

namespace tc::core {

class Server {
public:
  Server(asio::io_context &io_ctx, uint16_t port);

  void start();
  void stop();

private:
  void do_accept();
  void init_tls_context();

  asio::io_context &io_context_;
  asio::ip::tcp::acceptor acceptor_;
  asio::ssl::context ssl_context;
};

} // namespace tc::core
