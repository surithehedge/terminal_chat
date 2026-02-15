#include "server/net/secure_session.hpp"
#include "asio/bind_executor.hpp"
#include "asio/error_code.hpp"
#include "asio/read.hpp"
#include "common/logger.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace tc::net {
SecureSession::SecureSession(asio::ip::tcp::socket socket,
                             asio::ssl::context &ctx)
    : strand_(asio::make_strand(socket.get_executor())),
      ssl_socket_(std::move(socket), ctx),
      timer_(strand_.get_inner_executor()) {}

void SecureSession::start() {
  asio::dispatch(strand_,
                 [self = shared_from_this()]() { self->do_handshake(); });
}

void SecureSession::do_handshake() {
  timer_.expires_after(std::chrono::seconds(10));
  check_timeout();
  ssl_socket_.async_handshake(
      asio::ssl::stream_base::server,
      asio::bind_executor(
          strand_, [self = shared_from_this()](asio::error_code ec) {
            self->timer_.cancel();
            if (!ec) {
              tc::Logger::log(tc::LogLevel::INFO, "SESSION",
                              "TLS Handshake successful.");
              self->do_read_header();
            } else {
              tc::Logger::log(tc::LogLevel::WARN, "SESSION",
                              "Handshake failed: " + ec.message());
              self->stop();
            }
          }));
}
void SecureSession::check_timeout() {
  timer_.async_wait(asio::bind_executor(
      strand_, [self = shared_from_this()](asio::error_code ec) {
        if (!ec) {
          tc::Logger::log(tc::LogLevel::WARN, "SESSION",
                          "Handshake/Idle timeout reached.");
          self->stop();
        }
      }));
}
void SecureSession::stop() {
  asio::error_code ec;
  ssl_socket_.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
  ssl_socket_.lowest_layer().close(ec);
}

void SecureSession::do_read_header() {
  auto self = shared_from_this();
  asio::async_read(
      ssl_socket_, asio::buffer(header_buffer_, 8),
      asio::bind_executor(
          strand_, [self](asio::error_code ec, std::size_t /*length*/) {
            if (!ec) {
              if (self->header_buffer_[0] != 0x54 ||
                  self->header_buffer_[1] != 0x43) {
                tc::Logger::log(tc::LogLevel::WARN, "SESSION",
                                "Invalid magic number . Dropping Client");
                self->stop();
                return;
              }

              uint32_t body_len = 0;
              std::memcpy(&body_len, &self->header_buffer_[4], 4);
              body_len = ntohl(body_len);

              if (body_len > 65536) {
                tc::Logger::log(tc::LogLevel::WARN, "SESSION",
                                "Payload too large. Dropping Client");
                self->stop();
                return;
              }

              self->do_read_body(body_len);
            } else {
              tc::Logger::log(tc::LogLevel::INFO, "SESSION",
                              "Connection closed by peer.");
              self->stop();
            }
          }));
}

void SecureSession::do_read_body(uint32_t length) {
  if (length == 0) {
    do_read_header();
    return;
  }

  auto self = shared_from_this();
  auto body_buffer = std::make_shared<std::vector<uint8_t>>(length);

  asio::async_read(
      ssl_socket_, asio::buffer(body_buffer->data(), body_buffer->size()),
      asio::bind_executor(strand_, [self, body_buffer](asio::error_code ec,
                                                       std::size_t /*length*/) {
        if (!ec) {
          try {
            std::string json_raw(body_buffer->begin(), body_buffer->end());
            auto j = nlohmann::json::parse(json_raw);

            tc::Logger::log(tc::LogLevel::INFO, "SESSION",
                            "Received valid JSON frame.");

            nlohmann::json response = {{"status", "connected"},
                                       {"message", "Welcome to Terminal Chat"}};

            std::string resp_str = response.dump();
            std::vector<uint8_t> packet = {0x54, 0x43, 0x01, 0x01};

            uint32_t len = htonl(static_cast<uint32_t>(resp_str.size()));
            uint8_t len_bytes[4];
            std::memcpy(len_bytes, &len, 4);
            packet.insert(packet.end(), len_bytes, len_bytes + 4);
            packet.insert(packet.end(), resp_str.begin(), resp_str.end());

            self->write(std::move(packet));

            self->do_read_header();
          } catch (const std::exception &e) {
            tc::Logger::log(tc::LogLevel::WARN, "SESSION",
                            "JSON error: " + std::string(e.what()));
            self->do_read_header();
          }
        } else {
          self->stop();
        }
      }));
}

void SecureSession::write(const std::vector<uint8_t> &packet) {
  auto self = shared_from_this();
  asio::dispatch(strand_, [self, packet]() {
    if (self->write_queue_.size() >= 100) {
      tc::Logger::log(tc::LogLevel::WARN, "SESSION",
                      "Write queue exhausted. Terminating.");
      self->stop();
      return;
    }

    bool write_in_progress = !self->write_queue_.empty();
    self->write_queue_.push_back(std::move(packet));
    if (!write_in_progress) {
      self->do_write();
    }
  });
}

void SecureSession::do_write() {
  auto self = shared_from_this();

  asio::async_write(
      ssl_socket_, asio::buffer(write_queue_.front()),
      asio::bind_executor(strand_,
                          [self](asio::error_code ec, std::size_t /*length*/) {
                            if (!ec) {
                              self->write_queue_.pop_front();

                              if (!self->write_queue_.empty()) {
                                self->do_write();
                              }
                            } else {
                              tc::Logger::log(tc::LogLevel::WARN, "SESSION",
                                              "Write failed: " + ec.message());
                              self->stop();
                            }
                          }));
}
} // namespace tc::net
