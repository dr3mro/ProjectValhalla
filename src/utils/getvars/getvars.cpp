#include "getvars.hpp"
#include <cstdlib>
#include <fmt/format.h>
#include <iostream>
std::optional<std::string> EnvVars::get(const std::string& key) const
{
    const char* value = std::getenv(key.c_str());
    if (value) {
        return std::string(value);
    }
    return std::nullopt;
}

void EnvVars::set(const std::string& key, const std::string& value)
{
    std::string envVar = fmt::format("{}={}", key, value);
    if (putenv(const_cast<char*>(envVar.c_str())) != 0) {
        std::cerr << "Failed to set environment variable: " << key << std::endl;
    }
}
