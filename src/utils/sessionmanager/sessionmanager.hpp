#pragma once

#include "controllers/databasecontroller/databasecontroller.hpp"
#include "store/store.hpp"
#include <memory>
#include <string>

class SessionManager {
public:
    SessionManager(const std::string& tablename)
        : tablename(tablename)
    {
        databaseController = std::any_cast<std::shared_ptr<DatabaseController>>(Store::getObject(Type::DatabaseController));
    }

    virtual ~SessionManager() = default;
    void setNowLoginTime(uint64_t id);
    void setNowLogoutTime(uint64_t id);
    std::optional<std::string> getLastLoginTime(uint64_t id);
    std::optional<std::string> getLastLogoutTime(uint64_t id);

private:
    std::shared_ptr<DatabaseController> databaseController;
    std::string tablename;
    std::string current_time_to_utc_string();
};
