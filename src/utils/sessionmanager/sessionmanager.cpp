#include "sessionmanager.hpp"
#include "fmt/core.h"
#include <chrono>
#include <ctime>
#include <fmt/chrono.h>
#include <string>

void SessionManager::login(uint64_t id)
{
    try {
        std::string login_time = current_time_to_string();

        std::string query = fmt::format(
            "INSERT INTO user_sessions (id, last_login) VALUES ({}, '{}') "
            "ON CONFLICT (user_id) DO UPDATE SET last_login = EXCLUDED.last_login;",
            id, login_time);

        databaseController->executeQuery(query);
    } catch (const std::exception& e) {
        std::cerr << "Error updating login time: " << e.what() << std::endl;
    }
}

void SessionManager::logout(uint64_t id)
{
    try {
        std::string logout_time = current_time_to_string();

        std::string query = fmt::format(
            "INSERT INTO sessions (id, last_logout) VALUES ({}, '{}') "
            "ON CONFLICT (user_id) DO UPDATE SET last_logout = EXCLUDED.last_logout;",
            id, logout_time);

        databaseController->executeQuery(query);
    } catch (const std::exception& e) {
        std::cerr << "Error updating logout time: " << e.what() << std::endl;
    }
}

std::string current_time_to_utc_string()
{
    // Get current time as a time_point
    auto now = std::chrono::system_clock::now();

    // Convert time_point to time_t
    auto now_t = std::chrono::system_clock::to_time_t(now);

    // Convert time_t to tm structure in UTC
    std::tm tm = *std::gmtime(&now_t);

    // Format the time using fmt::format
    std::string formatted_time = fmt::format("{:%Y-%m-%d %H:%M:%S} +0000", tm);

    return formatted_time;
}