#include "utils/envloader/envloader.hpp"
#include <fmt/core.h>
#include <functional>
#include <sstream>
EnvLoader::EnvLoader(const EnvVars &env) : env(std::cref(env)) {
}

int EnvLoader::getIntEnv(const std::string &key, int defaultValue) {
    auto value = env.get(key);
    return value ? std::stoi(*value) : defaultValue;
}

std::chrono::seconds EnvLoader::getDurationEnv(const std::string &key, std::chrono::seconds defaultValue) {
    auto value = env.get(key);
    return value ? std::chrono::seconds(std::stoi(*value)) : defaultValue;
}

std::unordered_set<std::string> EnvLoader::getSetEnv(const std::string &key, const std::string &defaultValue) {
    auto value = env.get(key);
    return value ? parseSet(*value) : parseSet(defaultValue);
}

std::unordered_set<std::string> EnvLoader::parseSet(const std::string &str) {
    std::unordered_set<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // Trim any leading or trailing whitespace
        item.erase(item.find_last_not_of(" \n\r\t") + 1);
        item.erase(0, item.find_first_not_of(" \n\r\t"));
        result.insert(item);
    }
    return result;
}
