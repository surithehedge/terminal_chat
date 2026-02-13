#pragma once
#include <string>
#include <string_view>

namespace tc {

enum class LogLevel { INFO, WARN, ERR, AUDIT };

class Logger {
public:
    static void log(LogLevel level, std::string_view component, std::string_view message);
private:
    static std::string_view level_to_string(LogLevel level);
    static std::string get_timestamp();
};

} // namespace tc
