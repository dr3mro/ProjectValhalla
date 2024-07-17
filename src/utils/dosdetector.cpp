#include "utils/dosdetector.hpp"

DOSDetector::DOSDetector(size_t max_requests, std::chrono::seconds period)
    : max_requests_(max_requests)
    , period_(period)
{
    async_task_ = std::async(std::launch::async, &DOSDetector::cleanUpTask, this);
}

bool DOSDetector::id_dos_attack(const crow::request& req, crow::response& res)
{
    (void)res;
    try {
        auto ip = req.remote_ip_address;
        auto now = std::chrono::steady_clock::now();

        std::lock_guard<std::mutex> lock(m_);

        requests[ip].push_back(now);

        auto times = requests.at(ip);

        auto window = now - period_;

        times.erase(
            std::remove_if(times.begin(), times.end(),
                [window](const auto& time) { return time < window; }),
            times.end());

        if (times.size() > max_requests_) {
            return true;
        }
    } catch (std::exception& e) {
        std::cerr << "Failure" << e.what() << std::endl;
        return false;
    }
    return false;
}

void DOSDetector::cleanUpTask()
{
    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto next = now + std::chrono::seconds(600);

        {
            std::lock_guard<std::mutex> lock(m_);
            auto window = now - period_;

            for (auto it = requests.begin(); it != requests.end();) {
                auto& times = it->second;
                times.erase(
                    std::remove_if(times.begin(), times.end(),
                        [window](const auto& time) { return time < window; }),
                    times.end());

                if (times.empty()) {
                    it = requests.erase(it); // Remove the IP entry if no requests are left
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