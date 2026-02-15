#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>
#include "client/net/secure_client.hpp"
#include "common/logger.hpp"

int main() {
    try {
        asio::io_context io_ctx;
        asio::ssl::context ssl_ctx(asio::ssl::context::tls_client);
        ssl_ctx.set_verify_mode(asio::ssl::verify_none);

        auto client = std::make_shared<tc::net::SecureClient>(io_ctx, ssl_ctx);
        client->connect("127.0.0.1", "4433");
        std::thread net_thread([&io_ctx]() { io_ctx.run(); });

        std::cout << "Connected to server. Type a message and press Enter:\n";
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "/quit") break;
            nlohmann::json j = {{"text", line}};
            client->send(j.dump());
        }

        io_ctx.stop();
        if (net_thread.joinable()) net_thread.join();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
