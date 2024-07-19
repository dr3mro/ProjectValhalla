#include "dosdetector.hpp"
#include <fmt/format.h>
#include <iostream>
#include <thread>
#include <xxhash.h>

DOSDetector::DOSDetector(size_t max_requests, std::chrono::seconds period)
    : max_requests_(max_requests)
    , period_(period)
{
    async_task_ = std::async(std::launch::async, &DOSDetector::cleanUpTask, this);
}

DOSDetector::~DOSDetector()
{
    running_.store(false);
    if (async_task_.valid()) {
        async_task_.wait();
    }
}

bool DOSDetector::is_dos_attack(const crow::request& req, crow::response& res)
{
    (void)res;
    try {
        auto now = std::chrono::steady_clock::now();
        std::string key = generate_key(req); // ip + hash of reuqest (Header and Body).
        {
            std::lock_guard<std::mutex> lock(m_);
            auto& ip_requests = requests_[key];

            // Remove old requests that are outside the time window
            while (!ip_requests.empty() && ip_requests.front() < now - period_) {
                ip_requests.pop_front();
            }

            ip_requests.push_back(now);

            if (ip_requests.size() > max_requests_) {
                return true;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Failure: " << e.what() << std::endl;
        return false;
    }
    return false;
}

void DOSDetector::cleanUpTask()
{
    while (running_.load()) {
        auto now = std::chrono::steady_clock::now();
        auto next = now + std::chrono::seconds(600);
        auto window = now - period_;

        {
            std::lock_guard<std::mutex> lock(m_);

            for (auto it = requests_.begin(); it != requests_.end(); /* no increment here */) {
                auto& times = it->second;

                // Remove old requests that are outside the time window
                while (!times.empty() && times.front() < window) {
                    times.pop_front();
                }

                // If no requests are left for this key, erase the entry from the map
                if (times.empty()) {
                    it = requests_.erase(it); // Remove the entry if no requests are left
                } else {
                    ++it; // Increment iterator only if we did not erase
                }
            }
        }

        std::this_thread::sleep_until(next);
    }
}

std::string DOSDetector::generate_key(const crow::request& req)
{
    // Reserve space to avoid multiple reallocations
    std::string data;
    data.reserve(4096);

    // Append headers
    for (const auto& header : req.headers) {
        data.append(header.second);
    }

    // Append body
    data.append(req.body);

    // Hash the complete string using xxHash

    XXH64_hash_t hashed_key
        = XXH3_64bits(data.c_str(), data.size());

    // Format and return the final key
    return fmt::format("{}.{}", req.remote_ip_address, hashed_key);
}
