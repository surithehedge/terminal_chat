#include "common/logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace tc {


static std::mutex console_mutex;

std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm bt{};
#ifdef _WIN32
    localtime_s(&bt, &time_t_now);
#else
    localtime_r(&time_t_now, &bt);
#endif
    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string_view Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR:   return "ERR";
        case LogLevel::AUDIT: return "AUDIT";
        default:              return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, std::string_view component, std::string_view message) {
    std::lock_guard<std::mutex> lock(console_mutex);
    std::ostream& out = (level == LogLevel::ERR || level == LogLevel::AUDIT)
                        ? std::cerr
                        : std::cout;

    out << "[" << get_timestamp() << "] "
        << "[" << level_to_string(level) << "] "
        << "[" << component << "] "
        << message << std::endl;

    if (level == LogLevel::ERR) {
        out << "[FATAL] Process terminating due to ERROR level log." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

}
