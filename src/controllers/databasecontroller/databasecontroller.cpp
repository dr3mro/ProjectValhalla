#include "databasecontroller.hpp"
#include "store/store.hpp"
#include <fmt/core.h>

/**
 * Constructs a DatabaseController instance.
 *
 * The DatabaseController is responsible for managing the connection to the database
 * and executing queries. It retrieves the DatabaseConnectionPool instance from the
 * global Store and stores it for use in executing queries.
 *
 * If there is an exception while retrieving the DatabaseConnectionPool, an error
 * message is printed to stderr and the program exits with a failure status.
 */
DatabaseController::DatabaseController() {
    try {
        databaseConnectionPool = std::any_cast<std::shared_ptr<DatabaseConnectionPool>>(Store::getObject(Type::DatabaseConnectionPool));
    } catch (const std::exception &e) {
        std::cerr << "Exception in DatabaseController constructor: " << e.what() << std::endl;
        EXIT_FAILURE;
    }
}

/**
 * Executes a read-write SQL query against the database and returns the result as a JSON object.
 *
 * @param query The SQL query to execute.
 * @return An optional JSON object containing the query result, or an empty optional if the query failed.
 */
std::optional<json> DatabaseController::executeQuery(const std::string &query) {
    return executer<json>(&Database::executeQuery<pqxx::work>, query);
}

/**
 * Executes a read-only SQL query against the database and returns the result as a JSON object.
 *
 * @param query The SQL query to execute.
 * @return An optional JSON object containing the query result, or an empty optional if the query failed.
 */
std::optional<json> DatabaseController::executeReadQuery(const std::string &query) {
    return executer<json>(&Database::executeQuery<pqxx::nontransaction>, query);
}
/**
 * Executes a read-only SQL query against the database and returns the result as a string.
 *
 * @param query The SQL query to execute.
 * @return An optional string containing the query result, or an empty optional if the query failed.
 */
std::optional<std::string> DatabaseController::doReadQuery(const std::string &query) {
    return executer<std::string>(&Database::doSimpleQuery<std::string>, query);
}
/**
 * Checks if an item exists in the specified table and column with the given value.
 *
 * @param table The name of the database table to check.
 * @param column The name of the column to check.
 * @param value The value to check for in the specified column.
 * @return An optional boolean indicating whether the item exists (true) or not (false). If the check fails, the optional will be empty.
 */
std::optional<bool> DatabaseController::checkItemExists(const std::string &table, const std::string &column, const std::string &value) {
    return executer<bool>(&Database::checkExists, table, column, value).value();
}

/**
 * Retrieves the password hash for the user with the specified ID from the given table.
 *
 * @param user_id The ID of the user to retrieve the password hash for.
 * @param tablename The name of the database table to search.
 * @return An optional string containing the password hash, or an empty optional if the query fails.
 */
std::optional<std::string> DatabaseController::getPasswordHashForUserID(const uint64_t &user_id, const std::string &tablename) {
    // std::cout << fmt::format("SELECT password_hash FROM {} WHERE id = '{}' LIMIT 1;\n", tablename, user_id);
    return executer<std::string>(&Database::doSimpleQuery<std::string>, fmt::format("SELECT password_hash FROM {} WHERE id = '{}' LIMIT 1;", tablename, user_id));
}
/**
 * Finds the user ID for the specified username in the given database table and return 0 of not exists.
 *
 * @param username The username to search for.
 * @param tablename The name of the database table to search.
 * @return An optional uint64_t containing the user ID, or an empty optional if the user is not found.
 */
std::optional<uint64_t> DatabaseController::findIfUserID(const std::string &username, const std::string &tablename) {
    return executer<uint64_t>(&Database::doSimpleQuery<uint64_t>, fmt::format("SELECT id FROM {} WHERE username = '{}' LIMIT 1;", tablename, username));
}
