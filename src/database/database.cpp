#include "database.hpp"
#include <fmt/core.h>
#include <iostream>
#include <utility>

Database::Database(std::shared_ptr<pqxx::connection> conn)
    : connection(std::move(conn))
{
    if (connection->is_open()) {
        // std::cout << "Opened database successfully: " << conn->dbname() << std::endl;
    } else {
        std::cerr << "Failed to open database" << std::endl;
    }
}

bool Database::isConnected()
{
    return connection->is_open();
}

bool Database::checkExists(const std::string& table, const std::string& column, const std::string& value)
{
    try {
        pqxx::nontransaction txn(*connection);
        pqxx::result result = txn.exec(fmt::format("SELECT EXISTS ( SELECT 1 FROM {} WHERE {} = '{}');", table, column, value));
        return result[0][0].as<bool>();
    } catch (const std::exception& e) {
        std::cerr << "Error executing query: " << e.what() << std::endl;
        throw; // Rethrow the exception to indicate failure
    }
}
