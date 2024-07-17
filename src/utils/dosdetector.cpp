#include "utils/dosdetector.hpp"
#include <iostream>
#include <picosha2.h>
#include <thread>

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
        auto ip = req.remote_ip_address;
        auto now = std::chrono::steady_clock::now();
        std::string fingerprint = generate_fingerprint(req);

        std::lock_guard<std::mutex> lock(m_);

        auto& ip_requests = requests_[ip][fingerprint];
        ip_requests.push_back(now);

        auto window = now - period_;

        ip_requests.erase(
            std::remove_if(ip_requests.begin(), ip_requests.end(),
                [window](const auto& time) { return time < window; }),
            ip_requests.end());

        if (ip_requests.size() > max_requests_) {
            return true;
        }
    } catch (std::exception& e) {
        std::cerr << "Failure: " << e.what() << std::endl;
        return false;
    }
    return false;
}

void DOSDetector::cleanUpTask()
{
    while (running_.load()) {
        auto now = std::chrono::steady_clock::now();
        auto next = now + std::chrono::seconds(10);

        {
            std::lock_guard<std::mutex> lock(m_);
            auto window = now - period_;

            for (auto it = requests_.begin(); it != requests_.end();) {
                for (auto it_fp = it->second.begin(); it_fp != it->second.end();) {
                    auto& times = it_fp->second;
                    times.erase(
                        std::remove_if(times.begin(), times.end(),
                            [window](const auto& time) { return time < window; }),
                        times.end());

                    if (times.empty()) {
                        it_fp = it->second.erase(it_fp); // Remove the fingerprint entry if no requests are left
                    } else {
                        ++it_fp;
                    }
                }

                if (it->second.empty()) {
                    it = requests_.erase(it); // Remove the IP entry if no requests are left
                } else {
                    ++it;
                }
            }
        }
        auto elapsed_time = std::chrono::steady_clock::now() - now;
        CROW_LOG_INFO << "cleanUpTask: DOS Protection database cleanup complete. " << elapsed_time;
        std::this_thread::sleep_until(next);
    }
}

std::string DOSDetector::generate_fingerprint(const crow::request& req)
{
    std::stringstream fp;
    for (auto& header : req.headers) {
        fp << header.second;
    }
    fp << req.body;
    fp << req.remote_ip_address;
    return picosha2::hash256_hex_string(fp.str());
}