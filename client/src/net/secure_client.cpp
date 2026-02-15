#include "client/net/secure_client.hpp"
#include "common/logger.hpp"
#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace tc::net {

SecureClient::SecureClient(asio::io_context &io_ctx,
                           asio::ssl::context &ssl_ctx)
    : io_ctx_(io_ctx), resolver_(io_ctx), ssl_socket_(io_ctx, ssl_ctx) {}

void SecureClient::connect(const std::string &host, const std::string &port) {
  auto endpoints = resolver_.resolve(host, port);
  do_connect(endpoints);
}

void SecureClient::do_connect(asio::ip::tcp::resolver::results_type endpoints) {
  asio::async_connect(ssl_socket_.lowest_layer(), endpoints,
                      [this](asio::error_code ec, asio::ip::tcp::endpoint) {
                        if (!ec) {
                          tc::Logger::log(
                              tc::LogLevel::INFO, "CLIENT",
                              "TCP Connected. Starting TLS Handshake...");
                          do_handshake();
                        } else {
                          tc::Logger::log(tc::LogLevel::ERR, "CLIENT",
                                          "Connect failed: " + ec.message());
                        }
                      });
}

void SecureClient::do_handshake() {
  ssl_socket_.async_handshake(
      asio::ssl::stream_base::client, [this](asio::error_code ec) {
        if (!ec) {
          tc::Logger::log(tc::LogLevel::INFO, "CLIENT",
                          "TLS Handshake Success!");
          do_read_header();
        } else {
          tc::Logger::log(tc::LogLevel::ERR, "CLIENT",
                          "Handshake failed: " + ec.message());
        }
      });
}

void SecureClient::do_read_header() {
  ssl_socket_.async_read_some(
      asio::buffer(temp_buffer_),
      [this](asio::error_code ec, std::size_t length) {
        if (!ec) {
          read_accumulator_.insert(read_accumulator_.end(), temp_buffer_,
                                   temp_buffer_ + length);

          if (read_accumulator_.size() >= 8) {
            if (read_accumulator_[0] == 0x54 && read_accumulator_[1] == 0x43) {
              uint32_t body_len;
              std::memcpy(&body_len, &read_accumulator_[4], 4);
              body_len = ntohl(body_len);
              if (body_len > 65536) {
                tc::Logger::log(tc::LogLevel::ERR, "CLIENT",
                                "Payload too large. Disconnecting.");
                ssl_socket_.lowest_layer().close();
                return;
              }
              do_read_body(body_len);
            } else {
              tc::Logger::log(tc::LogLevel::ERR, "CLIENT",
                              "Protocol Mismatch (Magic). Clearing buffer.");
              read_accumulator_.clear();
              do_read_header();
            }
          } else {
            do_read_header();
          }
        } else {
          tc::Logger::log(tc::LogLevel::WARN, "CLIENT",
                          "Read failed: " + ec.message());
        }
      });
}

void SecureClient::do_read_body(uint32_t length) {
  if (read_accumulator_.size() >= (8 + length)) {
    std::string msg(read_accumulator_.begin() + 8,
                    read_accumulator_.begin() + 8 + length);
    std::cout << "\n[Server]: " << msg << std::endl;

    read_accumulator_.erase(read_accumulator_.begin(),
                            read_accumulator_.begin() + 8 + length);
    if (read_accumulator_.size() >= 8) {
      uint32_t next_len;
      std::memcpy(&next_len, &read_accumulator_[4], 4);
      do_read_body(ntohl(next_len));
    } else {
      do_read_header();
    }
  } else {
    do_read_header();
  }
}

void tc::net::SecureClient::send(const std::string &message) {
  std::vector<uint8_t> packet;
  packet.reserve(8 + message.size());

  packet.push_back(0x54);
  packet.push_back(0x43);

  packet.push_back(0x01);
  packet.push_back(0x01);

  uint32_t len = htonl(static_cast<uint32_t>(message.size()));
  uint8_t len_bytes[4];
  std::memcpy(len_bytes, &len, 4);
  for (int i = 0; i < 4; ++i)
    packet.push_back(len_bytes[i]);

  packet.insert(packet.end(), message.begin(), message.end());

  asio::post(io_ctx_, [this, p = std::move(packet)]() mutable {
    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(std::move(p));
    if (!write_in_progress) {
      do_write();
    }
  });
}

void tc::net::SecureClient::do_write() {
  asio::async_write(ssl_socket_, asio::buffer(write_queue_.front()),
                    [this](asio::error_code ec, std::size_t) {
                      if (!ec) {
                        write_queue_.pop_front();
                        if (!write_queue_.empty()) {
                          do_write();
                        }
                      } else {
                        tc::Logger::log(tc::LogLevel::ERR, "CLIENT",
                                        "Write failed: " + ec.message());
                      }
                    });
}
} // namespace tc::net
