#include <iostream>
#include <vector>
#include <thread>
#include <asio.hpp>
#include "server/core/server.hpp"
#include "common/logger.hpp"

int main() {
    try {
        asio::io_context io_ctx;

        auto server = std::make_unique<tc::core::Server>(io_ctx, 4433);

        server->start();

        unsigned int thread_count = std::thread::hardware_concurrency();
        if (thread_count == 0) thread_count = 1;

        tc::Logger::log(tc::LogLevel::INFO, "SERVER",
            "Launching thread pool with " + std::to_string(thread_count) + " threads.");

        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_ctx]() {
                io_ctx.run();
            });
        }
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

    } catch (const std::exception& e) {
        tc::Logger::log(tc::LogLevel::ERR, "FATAL",
            std::string("Server failed to start: ") + e.what());
        return 1;
    }

    return 0;
}
