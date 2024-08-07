#pragma once
#include "database.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
/**
 * @class DatabaseConnectionPool
 * @brief Manages a pool of database connections for efficient resource usage.
 *
 * The `DatabaseConnectionPool` class provides a way to manage a pool of database connections.
 * It allows clients to acquire a connection from the pool, use it, and then return it back to the pool
 * for reuse. This helps to optimize resource usage and avoid the overhead of creating and destroying
 * database connections for each request.
 *
 * The pool size is configurable and can be set when creating a new `DatabaseConnectionPool` instance.
 * The class uses a `std::queue` to store the available connections, and a `std::mutex` and
 * `std::condition_variable` to synchronize access to the pool.
 */
class DatabaseConnectionPool {
public:
    DatabaseConnectionPool(size_t pool_size);
    std::shared_ptr<Database> get_connection();
    void return_connection(std::shared_ptr<Database> db);

private:
    std::shared_ptr<Database> createDatabaseConnection();
    std::queue<std::shared_ptr<Database>> databaseConnections;
    std::mutex mutex;
    std::condition_variable cv;
};
