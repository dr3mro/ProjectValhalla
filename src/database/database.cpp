#include "database.hpp"
#include <fmt/core.h>
#include <iostream>
#include <utility>

/**
 * Constructs a Database object with the provided connection.
 *
 * @param conn A shared pointer to the PostgreSQL connection to use for database operations.
 */
Database::Database(std::shared_ptr<pqxx::connection> conn) : connection(std::move(conn)) {
    try {
        if (connection->is_open()) {
            // Execute a simple query to test the connection
            pqxx::work txn(*connection);
            txn.exec("SELECT 1");
            txn.commit();
        } else {
            std::cerr << "Failed to open database" << std::endl;
            EXIT_FAILURE;
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception caught during database connection: " << e.what() << std::endl;
        throw std::runtime_error("Failed to initialize database connection");
        EXIT_FAILURE;
    }
}

/**
 * Checks if the database connection is open.
 *
 * @return true if the database connection is open, false otherwise.
 */
bool Database::isConnected() {
    return connection->is_open();
}

/**
 * Checks if a record exists in the specified table and column with the given value.
 *
 * @param table The name of the table to check.
 * @param column The name of the column to check.
 * @param value The value to search for in the specified column.
 * @return true if a record exists, false otherwise.
 * @throws std::exception if there is an error executing the query.
 */
bool Database::checkExists(const std::string &table, const std::string &column, const std::string &value) {
    try {
        pqxx::nontransaction txn(*connection);
        pqxx::result result = txn.exec(fmt::format("SELECT EXISTS ( SELECT 1 FROM {} WHERE {} = '{}');", table, column, value));
        return result[0][0].as<bool>();
    } catch (const std::exception &e) {
        std::cerr << "Error executing query: " << e.what() << std::endl;
        throw; // Rethrow the exception to indicate failure
    }
}
