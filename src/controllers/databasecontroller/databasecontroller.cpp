#include "databasecontroller.hpp"
#include "store/store.hpp"
#include <fmt/core.h>

DatabaseController::DatabaseController() {
    try {
        databaseConnectionPool = std::any_cast<std::shared_ptr<DatabaseConnectionPool>>(Store::getObject(Type::DatabaseConnectionPool));
    } catch (const std::exception &e) {
        std::cerr << "Exception in DatabaseController constructor: " << e.what() << std::endl;
        EXIT_FAILURE;
    }
}

std::optional<json> DatabaseController::executeQuery(const std::string &query) {
    return executer<json>(&Database::executeQuery<pqxx::work>, query);
}

std::optional<json> DatabaseController::executeReadQuery(const std::string &query) {
    return executer<json>(&Database::executeQuery<pqxx::nontransaction>, query);
}
std::optional<std::string> DatabaseController::doReadQuery(const std::string &query) {
    return executer<std::string>(&Database::doSimpleQuery<std::string>, query);
}
std::optional<bool> DatabaseController::checkItemExists(const std::string &table, const std::string &column, const std::string &value) {
    return executer<bool>(&Database::checkExists, table, column, value).value();
}

std::optional<std::string> DatabaseController::getPasswordHashForUserID(const uint64_t &user_id, const std::string &tablename) {
    // std::cout << fmt::format("SELECT password_hash FROM {} WHERE id = '{}' LIMIT 1;\n", tablename, user_id);
    return executer<std::string>(&Database::doSimpleQuery<std::string>, fmt::format("SELECT password_hash FROM {} WHERE id = '{}' LIMIT 1;", tablename, user_id));
}
std::optional<uint64_t> DatabaseController::findIfUserID(const std::string &username, const std::string &tablename) {
    return executer<uint64_t>(&Database::doSimpleQuery<uint64_t>, fmt::format("SELECT id FROM {} WHERE username = '{}' LIMIT 1;", tablename, username));
}
