#include <iostream>
#include "common/logger.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>

int get_json_depth(const nlohmann::json& j) {
    if (!j.is_structured()) return 0;
    int max_child_depth = 0;
    for (auto it = j.begin(); it != j.end(); ++it) {
        max_child_depth = std::max(max_child_depth, get_json_depth(*it));
    }
    return 1 + max_child_depth;
}

int main() {
    tc::Logger::log(tc::LogLevel::INFO, "TEST", "Testing the logger foundation.");

    nlohmann::json deep_json = {
        {"level1", {
            {"level2", {
                {"level3", "data"}
            }}
        }}
    };

    int depth = get_json_depth(deep_json);
    if (depth > 3) {
        std::cerr << "JSON Depth Test Failed: Depth is " << depth << "\n";
        return 1;
    }

    std::cout << "Common tests passed (Depth: " << depth << ")\n";
    return 0;
}
