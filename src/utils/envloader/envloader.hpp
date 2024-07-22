#pragma once

#include "utils/envvars/envvars.hpp"
#include <chrono>
#include <string>
#include <unordered_set>

class EnvLoader {
public:
    explicit EnvLoader(const EnvVars& env);
    virtual ~EnvLoader() = default;

    int getIntEnv(const std::string& key, int defaultValue);
    std::chrono::seconds getDurationEnv(const std::string& key, std::chrono::seconds defaultValue);
    std::unordered_set<std::string> getSetEnv(const std::string& key, const std::string& defaultValue);

    virtual void printValues() const = 0;

protected:
    EnvVars env;

    std::unordered_set<std::string> parseSet(const std::string& str);

    virtual void initialize() = 0;
};
