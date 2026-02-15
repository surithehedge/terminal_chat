#include "server/core/server.hpp"
#include "server/net/secure_session.hpp"
#include "asio/ssl/context.hpp"
#include "common/logger.hpp"
#include <memory>
#include <string>
#include <utility>

namespace tc::core {
Server::Server(asio::io_context &io_ctx, uint16_t port)
    : io_context_(io_ctx),
      acceptor_(io_ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      ssl_context(asio::ssl::context::tls_server) {

  init_tls_context();
  tc::Logger::log(tc::LogLevel::INFO, "SERVER",
                  "Initialized on port" + std::to_string(port));
}

void Server::init_tls_context() {
  ssl_context.set_options(
      asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 |
      asio::ssl::context::no_sslv3 | asio::ssl::context::no_tlsv1 |
      asio::ssl::context::no_tlsv1_1 | asio::ssl::context::no_compression);

  tc::Logger::log(tc::LogLevel::INFO, "SECURITY",
                  "TLS 1.2+ context configured.");

  try {
          ssl_context.use_certificate_chain_file("server.pem");

          ssl_context.use_private_key_file("key.pem", asio::ssl::context::pem);

          tc::Logger::log(tc::LogLevel::INFO, "SECURITY", "TLS certificates loaded successfully.");
      } catch (const std::exception& e) {
          tc::Logger::log(tc::LogLevel::ERR, "SECURITY",
              std::string("Failed to load certificates: ") + e.what());
      }
}

void Server::start() {
  tc::Logger::log(tc::LogLevel::INFO, "SERVER", "Starting acceptor loop...");
  do_accept();
}

void Server::do_accept() {
  acceptor_.async_accept([this](asio::error_code ec,
                                asio::ip::tcp::socket socket) {
    if (!ec) {
      tc::Logger::log(tc::LogLevel::INFO, "SERVER",
                      "New connection from " +
                          socket.remote_endpoint().address().to_string());
      std::make_shared<tc::net::SecureSession>(std::move(socket), ssl_context)
          ->start();
    } else {
      tc::Logger::log(tc::LogLevel::WARN, "SERVER",
                      "Accept error:" + ec.message());
    }

    do_accept();
  });
}

} // namespace tc::core
