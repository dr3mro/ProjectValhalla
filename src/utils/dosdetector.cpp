#include "dosdetector.hpp"
#include <fmt/format.h>
#include <iostream>
#include <regex>
#include <thread>
#include <xxhash.h>

DOSDetector::DOSDetector()
{
    async_task_clean_ = std::async(std::launch::async, &DOSDetector::cleanUpTask, this);
}

DOSDetector::~DOSDetector()
{
    running_clean_.store(false);
    if (async_task_clean_.valid()) {
        async_task_clean_.wait();
    }

    if (async_task_process_.valid()) {
        async_task_process_.wait();
    }
}

DOSDetector::Status DOSDetector::is_dos_attack(const crow::request& req)
{
    try {
        if (isWhitelisted(req.remote_ip_address)) {
            return Status::WHITELISTED;
        }
        if (isBlacklisted(req.remote_ip_address)) {
            return Status::BLACKLISTED;
        }

        if (isRateLimited(req.remote_ip_address)) {
            async_task_process_ = std::async(std::launch::async, &DOSDetector::processRequest<crow::request>, this, req);
            return Status::RATELIMITED;
        }

        if (isBanned(req.remote_ip_address)) {
            async_task_process_ = std::async(std::launch::async, &DOSDetector::processRequest<crow::request>, this, req);
            return Status::BANNED;
        }

        return processRequest<const crow::request&>(std::cref(req));
    } catch (const std::exception& e) {
        std::cerr << "Failure: " << e.what() << std::endl;
        return Status::ERROR;
    }
    return Status::ALLOWED;
}

void DOSDetector::cleanUpTask()
{
    while (running_clean_.load()) {
        auto now = std::chrono::steady_clock::now();
        auto next = now + std::chrono::seconds(CLN_FRQ_);
        auto window = now - period_;
        // Cleanup requests and blocked IPs
        {
            std::lock_guard<std::mutex> request_lock(request_mutex_);
            // Cleanup requests
            for (auto& ot : requests_) {
                auto& requests = ot.second;

                for (auto it = requests.begin(); it != requests.end(); /* no increment here */) {
                    auto& times = it->second;

                    while (!times.empty() && times.front() < window) {
                        times.pop_front();
                    }

                    if (times.empty()) {
                        it = requests.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        {
            std::lock_guard<std::mutex> block_lock(ratelimit_mutex_);
            for (auto it = ratelimited_ips_.begin(); it != ratelimited_ips_.end(); /* no increment here */) {
                if (now >= it->second) {
                    it = ratelimited_ips_.erase(it); // Unblock IP
                } else {
                    ++it;
                }
            }
        }

        {
            std::lock_guard<std::mutex> ban_lock(ban_mutex_);
            for (auto it = banned_ips_.begin(); it != banned_ips_.end(); /* no increment here */) {
                if (now >= it->second) {
                    it = banned_ips_.erase(it); // Unblock IP
                } else {
                    ++it;
                }
            }
        }

        std::this_thread::sleep_until(next);
    }
}

std::string DOSDetector::generateRequestFingerprint(const crow::request& req)
{
    std::string data;
    data.reserve(4096);

    for (const auto& header : req.headers) {
        data.append(header.second);
    }

    data.append(req.body);

    XXH64_hash_t hashed_key = XXH3_64bits(data.c_str(), data.size());

    return fmt::format("{}", hashed_key);
}
bool DOSDetector::isWhitelisted(const std::string& remote_ip)
{
    return regexFind(remote_ip, whitelist_, whitelist_mutex_);
}
bool DOSDetector::isBlacklisted(const std::string& remote_ip)
{
    return regexFind(remote_ip, blacklist_, blacklist_mutex_);
}
bool DOSDetector::regexFind(const std::string& remote_ip, const std::unordered_set<std::string>& list, std::mutex& mtx)
{
    std::lock_guard<std::mutex> lock(mtx);

    return std::any_of(list.begin(), list.end(), [&remote_ip](const std::string& pattern) {
        try {
            std::regex regex_pattern(pattern);
            return std::regex_search(remote_ip, regex_pattern);
        } catch (const std::regex_error& e) {
            std::cerr << "Invalid regex pattern: " << e.what() << std::endl;
            return false;
        }
    });
}

bool DOSDetector::isBanned(const std::string& remote_ip)
{
    return checkStatus(remote_ip, banned_ips_, ban_mutex_);
}

bool DOSDetector::isRateLimited(const std::string& remote_ip)
{
    return checkStatus(remote_ip, ratelimited_ips_, ratelimit_mutex_);
}

template <typename Map, typename Mutex>
bool DOSDetector::checkStatus(const std::string& remote_ip, Map& ip_map, Mutex& mtx)
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<Mutex> lock(mtx);

    if (ip_map.find(remote_ip) != ip_map.end()) {
        auto forget_time = ip_map[remote_ip];
        if (now < forget_time) {
            return true;
        } else {
            ip_map.erase(remote_ip);
        }
    }
    return false;
}

template <typename Req>
DOSDetector::Status DOSDetector::processRequest(Req&& req)
{
    try {
        auto now = std::chrono::steady_clock::now();
        std::string remote_ip = req.remote_ip_address;
        std::string request_fingerprint = generateRequestFingerprint(req);

        {
            std::lock_guard<std::mutex> request_lock(request_mutex_);
            auto& ip_requests = requests_[remote_ip];
            auto& fp_requests = ip_requests[request_fingerprint];

            // Remove old requests that are outside the time window
            while (!fp_requests.empty() && fp_requests.front() < now - period_) {
                fp_requests.pop_front();
            }

            fp_requests.push_back(now);

            if (ip_requests.size() > max_fingerprints_) {
                std::lock_guard<std::mutex> block_lock(ratelimit_mutex_);
                ratelimited_ips_[remote_ip] = now + ratelimit_duration_;
                return Status::RATELIMITED;
            }

            if (fp_requests.size() > max_requests_) {
                std::lock_guard<std::mutex> ban_lock(ban_mutex_);
                banned_ips_[remote_ip] = now + ban_duration_;
                return Status::BANNED;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Failure: " << e.what() << std::endl;
        return Status::ERROR;
    }
    return Status::ALLOWED;
}