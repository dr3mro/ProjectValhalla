#include "databasecontroller.hpp"
#include <fmt/core.h>
#include <iostream>

DatabaseController::DatabaseController(std::shared_ptr<DatabaseConnectionPool> dbConnPool)
    : databaseConnectionPool(std::move(dbConnPool))
{
}

json DatabaseController::executeQuery(const std::string& query)
{
    std::shared_ptr<Database> db = nullptr;
    try {
        db = databaseConnectionPool->get_connection();
        json results = db->executeQuery(std::ref(query));
        databaseConnectionPool->return_connection(std::move(db));
        return results;
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred during executeQuery: " << e.what() << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    } catch (...) {
        std::cerr << "Unknown exception occurred during executeQuery." << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    }
    return json(); // Return empty JSON object on failure (if no rethrow)
}

json DatabaseController::executeReadQuery(const std::string& query)
{
    std::shared_ptr<Database> db = nullptr;
    try {
        db = databaseConnectionPool->get_connection();
        json results = db->executeReadQuery(std::ref(query));
        databaseConnectionPool->return_connection(std::move(db));
        return results;
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred duringexecuteReadQuery: " << e.what() << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    } catch (...) {
        std::cerr << "Unknown exception occurred during executeReadQuery." << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    }
    return json(); // Return empty JSON object on failure (if no rethrow)
}

bool DatabaseController::checkItemExists(const std::string& table, const std::string& column, const std::string& value)
{
    std::shared_ptr<Database> db = nullptr;
    try {
        db = databaseConnectionPool->get_connection();
        bool results = db->checkExists(table, column, value);
        databaseConnectionPool->return_connection(std::move(db));
        return results;
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred duringexecuteReadQuery: " << e.what() << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    } catch (...) {
        std::cerr << "Unknown exception occurred during executeReadQuery." << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    }
    return false; // Return empty JSON object on failure (if no rethrow)
}

std::string DatabaseController::getPasswordHashForUserID(const uint64_t& user_id)
{
    std::shared_ptr<Database> db = nullptr;
    try {
        db = databaseConnectionPool->get_connection();
        std::string passwordHash = db->doSimpleQuery<std::string>(fmt::format("SELECT password_hash FROM users WHERE user_id = '{}' LIMIT 1;", user_id));
        databaseConnectionPool->return_connection(std::move(db));
        return passwordHash;
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred duringexecuteReadQuery: " << e.what() << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    } catch (...) {
        std::cerr << "Unknown exception occurred during executeReadQuery." << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    }
    return std::string(); // Return empty JSON object on failure (if no rethrow)
}
uint64_t DatabaseController::findIfUserID(const std::string& username)
{
    std::shared_ptr<Database> db = nullptr;
    try {
        db = databaseConnectionPool->get_connection();
        uint64_t user_id = db->doSimpleQuery<uint64_t>(fmt::format("SELECT user_id FROM users WHERE username = '{}' LIMIT 1;", username));
        databaseConnectionPool->return_connection(db);
        return user_id;
    } catch (const std::exception& e) {
        std::cerr << "findIfUserID: Exception." << e.what() << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    } catch (...) {
        std::cerr << "Unknown exception occurred (findIfUserID)." << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    }
    return 0; // Return empty JSON object on failure (if no rethrow)
}
