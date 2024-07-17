#pragma once
#include <atomic>
#include <chrono>
#include <crow.h>
#include <future>
#include <mutex>
#include <unordered_map>

class DOSDetector {
public:
    DOSDetector(size_t max_requests, std::chrono::seconds period);
    ~DOSDetector() = default;
    bool id_dos_attack(const crow::request& req, crow::response& res);

private:
    size_t max_requests_;
    std::chrono::seconds period_;
    std::unordered_map<std::string, std::vector<std::chrono::steady_clock::time_point>> requests;
    std::mutex m_;
    std::future<void> async_task_;
    void cleanUpTask();
};
