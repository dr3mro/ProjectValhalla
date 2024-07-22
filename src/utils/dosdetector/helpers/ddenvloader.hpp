#pragma once
#include "utils/envloader/envloader.hpp"
#include <chrono>
#include <fmt/core.h>
#include <string>
#include <unordered_set>

// Constants for default values
constexpr int MAX_REQUESTS_ = 10;
constexpr int PERIOD_ = 30;
constexpr int MAX_FPS_ = 30;
constexpr int RL_DURATION_ = 30;
constexpr int BAN_DURATION_ = 3600;
constexpr int CLN_FRQ_ = 30;
const std::unordered_set<std::string> WHITELIST_ = { "127.0.1.*" };
const std::unordered_set<std::string> BLACKLIST_ = { "127.0.1.*" };

class DOSDetectorEnvLoader : public EnvLoader {
public:
    DOSDetectorEnvLoader(EnvVars& env)
        : EnvLoader(env)
    {
        initialize();
    }
    ~DOSDetectorEnvLoader() = default;

    void printValues() const override
    {
        fmt::print("Max Requests: {}\n", config_.max_requests);
        fmt::print("Period: {} seconds\n", config_.period.count());
        fmt::print("Max FPS: {}\n", config_.max_fingerprints);
        fmt::print("Rate Limit Duration: {} seconds\n", config_.ratelimit_duration.count());
        fmt::print("Ban Duration: {} seconds\n", config_.ban_duration.count());
        fmt::print("Cleanup Frequency: {} seconds\n", config_.clean_freq);
    }

    struct Config {
        size_t max_requests;
        std::chrono::seconds period;
        size_t max_fingerprints;
        std::chrono::seconds ratelimit_duration;
        std::chrono::seconds ban_duration;
        size_t clean_freq;
        std::unordered_set<std::string> whitelist;
        std::unordered_set<std::string> blacklist;
    };

    const Config& getConfig() const
    {
        return config_;
    }

    void setConfig(const Config& config)
    {
        config_ = config;
    }

private:
    Config config_;

    void initialize() override
    {
        config_.max_requests = getIntEnv("MAX_REQUESTS", MAX_REQUESTS_);
        config_.period = getDurationEnv("PERIOD", std::chrono::seconds(PERIOD_));
        config_.max_fingerprints = getIntEnv("MAX_FPS", MAX_FPS_);
        config_.ratelimit_duration = getDurationEnv("RL_DURATION", std::chrono::seconds(RL_DURATION_));
        config_.ban_duration = getDurationEnv("BAN_DURATION", std::chrono::seconds(BAN_DURATION_));
        config_.clean_freq = getIntEnv("CLN_FRQ", CLN_FRQ_);
        config_.whitelist = getSetEnv("WHITELIST", "");
        config_.blacklist = getSetEnv("BLACKLIST", "");
        printValues();
    }
};
