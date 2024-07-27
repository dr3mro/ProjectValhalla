#pragma once

#include "controllers/databasecontroller/databasecontroller.hpp"
#include <memory>
#include <string>

class SessionManager {
public:
    SessionManager(const std::shared_ptr<DatabaseController>& databaseController)
        : databaseController(databaseController)
    {
    }

    virtual ~SessionManager() = default;
    void login(uint64_t user_id);
    void logout(uint64_t user_id);

private:
    std::shared_ptr<DatabaseController> databaseController;
    std::string current_time_to_string();
};
