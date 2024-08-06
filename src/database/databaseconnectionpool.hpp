#pragma once
#include "database.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
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
