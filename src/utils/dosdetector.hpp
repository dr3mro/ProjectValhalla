#pragma once
#include <atomic>
#include <chrono>
#include <crow.h>
#include <deque>
#include <future>
#include <mutex>
#include <unordered_map>

class DOSDetector {
public:
    DOSDetector(size_t max_requests, std::chrono::seconds period);
    ~DOSDetector();
    bool is_dos_attack(const crow::request& req, crow::response& res);

private:
    size_t max_requests_;
    std::chrono::seconds period_;
    std::unordered_map<std::string, std::deque<std::chrono::steady_clock::time_point>> requests_;
    std::mutex m_;
    std::future<void> async_task_;
    std::atomic<bool> running_ { true };

    void cleanUpTask();
    std::string generate_key(const crow::request& req);
};
