#pragma once

#include <optional>
#include <string>

class EnvVars {
public:
    // Method to get an environment variable by name
    std::optional<std::string> get(const std::string &key) const;

    // Method to set an environment variable
    void set(const std::string &key, const std::string &value);
};
