#pragma once
#include "utils/dosdetector/helpers/ddenvloader.hpp"
#include "utils/envvars/envvars.hpp"
#include <atomic>
#include <chrono>
#include <crow.h>
#include <deque>
#include <future>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

class DOSDetector {
public:
    enum Status : uint8_t {
        ALLOWED,
        WHITELISTED,
        BLACKLISTED,
        RATELIMITED,
        BANNED,
        ERROR
    };

    DOSDetector();
    ~DOSDetector();
    DOSDetector::Status is_dos_attack(const crow::request& req);

private:
    EnvVars ev;
    DOSDetectorEnvLoader dosDetectorEnvLoader;

    DOSDetectorEnvLoader::Config config;

    //                     // IP                       // Hash of Request                // times of requests
    std::unordered_map<std::string, std::unordered_map<std::string, std::deque<std::chrono::steady_clock::time_point>>> requests_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> ratelimited_ips_; // time to unblock
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> banned_ips_; // time to unban
    std::unordered_set<std::string> whitelist_ = config.whitelist;
    std::unordered_set<std::string> blacklist_ = config.blacklist;

    std::mutex request_mutex_;
    std::mutex ratelimit_mutex_;
    std::mutex ban_mutex_;
    std::mutex whitelist_mutex_;
    std::mutex blacklist_mutex_;

    std::future<void> async_task_clean_;
    std::atomic<bool> running_clean_ { true };
    std::future<Status> async_task_process_;

    void cleanUpTask();
    std::string generateRequestFingerprint(const crow::request& req);
    bool isWhitelisted(const std::string& remote_ip);
    bool isBlacklisted(const std::string& remote_ip);
    bool regexFind(const std::string& remote_ip, const std::unordered_set<std::string>& list, std::mutex& mtx);
    bool isBanned(const std::string& remote_ip);
    bool isRateLimited(const std::string& remote_ip);
    template <typename Map, typename Mutex>
    bool checkStatus(const std::string& remote_ip, Map& ip_map, Mutex& mtx);

    template <typename Req>
    DOSDetector::Status processRequest(Req&& req);
};
