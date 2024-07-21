#pragma once
#include <atomic>
#include <chrono>
#include <crow.h>
#include <deque>
#include <future>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#define MAX_REQUESTS_ 30
#define PERIOD_ std::chrono::seconds(30)
#define MAX_FPS_ 30
#define RL_DURATION_ std::chrono::seconds(30)
#define BAN_DURATION_ std::chrono::seconds(3600)
#define CLN_FRQ_ 30
#define WHITELIST { "127.0.1.*" }
#define BLACKLIST { "127.0.1.*" }

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
    size_t max_requests_ = MAX_REQUESTS_;
    std::chrono::seconds period_ = PERIOD_;
    size_t max_fingerprints_ = MAX_FPS_;
    std::chrono::seconds ratelimit_duration_ = RL_DURATION_;
    std::chrono::seconds ban_duration_ = BAN_DURATION_;
    uint32_t clean_freq_ = CLN_FRQ_;

    //                     // IP                       // Hash of Request                // times of requests
    std::unordered_map<std::string, std::unordered_map<std::string, std::deque<std::chrono::steady_clock::time_point>>> requests_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> ratelimited_ips_; // time to unblock
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> banned_ips_; // time to unban
    std::unordered_set<std::string> whitelist_ WHITELIST;
    std::unordered_set<std::string> blacklist_ BLACKLIST;

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
