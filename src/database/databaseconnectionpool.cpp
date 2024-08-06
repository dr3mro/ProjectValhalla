#include "databaseconnectionpool.hpp"
#include <future>
#include <iostream>
#include <pqxx/pqxx> // Assuming pqxx library is used


// Define constants for database connection
const char *DB_HOST = "172.20.0.2";
const char *DB_NAME = "postgres";
const char *DB_USER = "postgres";
const char *DB_PASSWORD = "postgres";

/**
 * Initializes a database connection pool with the specified number of connections.
 *
 * This constructor creates a pool of database connections using the provided
 * database connection parameters. It attempts to establish a connection for
 * each connection in the pool, and adds the successful connections to the pool.
 * If any connection fails to open, an exception is thrown.
 *
 * @param pool_size The number of database connections to create in the pool.
 */
std::shared_ptr<Database> DatabaseConnectionPool::createDatabaseConnection() {
    auto conn = std::make_shared<pqxx::connection>(
        "host=" + std::string(DB_HOST) + " dbname=" + std::string(DB_NAME) + " user=" + std::string(DB_USER) + " password=" + std::string(DB_PASSWORD) +
        " connect_timeout=2");

    if (conn->is_open()) {
        return std::make_shared<Database>(conn);
    } else {
        throw std::runtime_error("Failed to open database connection");
    }
}

DatabaseConnectionPool::DatabaseConnectionPool(size_t pool_size) {
    try {
        for (size_t i = 0; i < pool_size; ++i) {
            auto future = std::async(std::launch::async, &DatabaseConnectionPool::createDatabaseConnection, this);

            auto status = future.wait_for(std::chrono::seconds(3)); // Set a timeout of 2 seconds
            if (status == std::future_status::ready) {
                databaseConnections.push(future.get());
            } else {
                throw std::runtime_error("Connection attempt timed out, failed to open database connection");
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception caught during database connection pool initialization: " << e.what() << std::endl;
        throw std::runtime_error("Failed to initialize database connection pool");
    }
}


// Get a connection from the pool
/**
 * Retrieves a database connection from the connection pool.
 *
 * This function acquires a lock on the connection pool mutex and waits for a
 * connection to become available in the pool. Once a connection is available,
 * it is removed from the pool and returned.
 *
 * @return A shared pointer to a Database object representing the retrieved
 * database connection.
 */
std::shared_ptr<Database> DatabaseConnectionPool::get_connection() {
    std::unique_lock<std::mutex> lock(mutex);
    while (databaseConnections.empty()) {
        cv.wait(lock);
    }
    auto db = databaseConnections.front();
    databaseConnections.pop();
    // std::cout << "Database connection retrieved from pool. Connections remaining: " << databaseConnections.size() << std::endl;
    return db;
}

// Return a connection to the pool
/**
 * Returns a database connection to the connection pool.
 *
 * This function acquires a lock on the connection pool mutex and adds the
 * provided database connection back to the pool. It then notifies any waiting
 * threads that a connection is available.
 *
 * @param db The database connection to return to the pool.
 */
void DatabaseConnectionPool::return_connection(std::shared_ptr<Database> db) {
    std::lock_guard<std::mutex> lock(mutex);
    databaseConnections.push(std::move(db));
    cv.notify_one();
    // std::cout << "Database connection returned to pool. Connections available: " << databaseConnections.size() << std::endl;
}
