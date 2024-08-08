#include "dosdetector.hpp"
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <regex>
#include <thread>
#include <xxhash.h>

DOSDetector::DOSDetector() : dosDetectorEnvLoader(DOSDetectorEnvLoader(std::cref(ev))) {
    try {
        config = dosDetectorEnvLoader.getConfig();
        async_task_clean_ = std::async(std::launch::async, &DOSDetector::cleanUpTask, this);
    } catch (const std::exception &e) {
        std::cerr << "Exception during DOSDetector initialization: " << e.what() << std::endl;
    }
}

DOSDetector::~DOSDetector() {
    try {
        running_clean_.store(false);
        if (async_task_clean_.valid()) {
            async_task_clean_.wait();
        }

        if (async_task_process_.valid()) {
            async_task_process_.wait();
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception during DOSDetector destruction: " << e.what() << std::endl;
    }
}

DOSDetector::Status DOSDetector::is_dos_attack(const crow::request &req) {
    try {
        if (isWhitelisted(std::cref(req.remote_ip_address))) {
            return Status::WHITELISTED;
        }
        if (isBlacklisted(std::cref(req.remote_ip_address))) {
            return Status::BLACKLISTED;
        }

        if (isRateLimited(std::cref(req.remote_ip_address))) {
            async_task_process_ = std::async(std::launch::async, &DOSDetector::processRequest<crow::request>, this, req);
            return Status::RATELIMITED;
        }

        if (isBanned(std::cref(req.remote_ip_address))) {
            async_task_process_ = std::async(std::launch::async, &DOSDetector::processRequest<crow::request>, this, req);
            return Status::BANNED;
        }

        return processRequest<const crow::request &>(std::cref(req));
    } catch (const std::exception &e) {
        std::cerr << "Failure in is_dos_attack: " << e.what() << std::endl;
        return Status::ERROR;
    }
}

void DOSDetector::cleanUpTask() {
    try {
        while (running_clean_.load()) {
            auto now = std::chrono::steady_clock::now();
            auto next = now + std::chrono::seconds(config.clean_freq);
            auto window = now - config.period;

            // Cleanup requests and blocked IPs
            {
                std::lock_guard<std::mutex> request_lock(request_mutex_);
                // Cleanup requests
                for (auto &ot : requests_) {
                    auto &requests = ot.second;

                    for (auto it = requests.begin(); it != requests.end(); /* no increment here */) {
                        auto &times = it->second;

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
    } catch (const std::exception &e) {
        std::cerr << "Exception in cleanUpTask: " << e.what() << std::endl;
    }
}

std::string DOSDetector::generateRequestFingerprint(const crow::request &req) {
    try {
        std::string data;
        data.reserve(4096);

        for (const auto &header : req.headers) {
            data.append(header.second);
        }

        data.append(req.body);

        XXH64_hash_t hashed_key = XXH3_64bits(data.c_str(), data.size());

        return fmt::format("{}", hashed_key);
    } catch (const std::exception &e) {
        std::cerr << "Exception in generateRequestFingerprint: " << e.what() << std::endl;
        return "";
    }
}

bool DOSDetector::isWhitelisted(const std::string &remote_ip) {
    try {
        return regexFind(remote_ip, whitelist_, whitelist_mutex_);
    } catch (const std::exception &e) {
        std::cerr << "Exception in isWhitelisted: " << e.what() << std::endl;
        return false;
    }
}

bool DOSDetector::isBlacklisted(const std::string &remote_ip) {
    try {
        return regexFind(remote_ip, blacklist_, blacklist_mutex_);
    } catch (const std::exception &e) {
        std::cerr << "Exception in isBlacklisted: " << e.what() << std::endl;
        return false;
    }
}

bool DOSDetector::regexFind(const std::string &remote_ip, const std::unordered_set<std::string> &list, std::mutex &mtx) {
    try {
        std::lock_guard<std::mutex> lock(mtx);

        return std::any_of(list.begin(), list.end(), [&remote_ip](const std::string &pattern) {
            try {
                std::regex regex_pattern(pattern);
                return std::regex_search(remote_ip, regex_pattern);
            } catch (const std::regex_error &e) {
                std::cerr << "Invalid regex pattern: " << e.what() << std::endl;
                return false;
            }
        });
    } catch (const std::exception &e) {
        std::cerr << "Exception in regexFind: " << e.what() << std::endl;
        return false;
    }
}

bool DOSDetector::isBanned(const std::string &remote_ip) {
    try {
        return checkStatus(remote_ip, banned_ips_, ban_mutex_);
    } catch (const std::exception &e) {
        std::cerr << "Exception in isBanned: " << e.what() << std::endl;
        return false;
    }
}

bool DOSDetector::isRateLimited(const std::string &remote_ip) {
    try {
        return checkStatus(remote_ip, ratelimited_ips_, ratelimit_mutex_);
    } catch (const std::exception &e) {
        std::cerr << "Exception in isRateLimited: " << e.what() << std::endl;
        return false;
    }
}

template <typename Map, typename Mutex>
bool DOSDetector::checkStatus(const std::string &remote_ip, Map &ip_map, Mutex &mtx) {
    try {
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
    } catch (const std::exception &e) {
        std::cerr << "Exception in checkStatus: " << e.what() << std::endl;
        return false;
    }
}

template <typename Req>
DOSDetector::Status DOSDetector::processRequest(Req &&req) {
    try {
        auto now = std::chrono::steady_clock::now();
        std::string remote_ip = req.remote_ip_address;
        std::string request_fingerprint = generateRequestFingerprint(req);

        {
            std::lock_guard<std::mutex> request_lock(request_mutex_);
            auto &ip_requests = requests_[remote_ip];
            auto &fp_requests = ip_requests[request_fingerprint];

            // Remove old requests that are outside the time window
            while (!fp_requests.empty() && fp_requests.front() < now - config.period) {
                fp_requests.pop_front();
            }

            fp_requests.push_back(now);

            if (ip_requests.size() > config.max_fingerprints) {
                std::lock_guard<std::mutex> block_lock(ratelimit_mutex_);
                ratelimited_ips_[remote_ip] = now + config.ratelimit_duration;
                return Status::RATELIMITED;
            }

            if (fp_requests.size() > config.max_requests) {
                std::lock_guard<std::mutex> ban_lock(ban_mutex_);
                banned_ips_[remote_ip] = now + config.ban_duration;
                return Status::BANNED;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Failure in processRequest: " << e.what() << std::endl;
        return Status::ERROR;
    }
    return Status::ALLOWED;
}
