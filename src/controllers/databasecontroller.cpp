#include "databasecontroller.hpp"
#include <fmt/core.h>

DatabaseController::DatabaseController(std::shared_ptr<DatabaseConnectionPool> dbConnPool)
    : databaseConnectionPool(std::move(dbConnPool))
{
}

json DatabaseController::executeQuery(const std::string& query)
{
    return executer<json>(&Database::executeQuery, query);
}

json DatabaseController::executeReadQuery(const std::string& query)
{
    return executer<json>(&Database::executeReadQuery, query);
}

bool DatabaseController::checkItemExists(const std::string& table, const std::string& column, const std::string& value)
{
    return executer<bool>(&Database::checkExists, table, column, value);
}

std::string DatabaseController::getPasswordHashForUserID(const uint64_t& user_id)
{
    return executer<std::string>(&Database::doSimpleQuery<std::string>, fmt::format("SELECT password_hash FROM users WHERE user_id = '{}' LIMIT 1;", user_id));
}
uint64_t DatabaseController::findIfUserID(const std::string& username)
{
    return executer<uint64_t>(&Database::doSimpleQuery<uint64_t>, fmt::format("SELECT user_id FROM users WHERE username = '{}' LIMIT 1;", username));
}
