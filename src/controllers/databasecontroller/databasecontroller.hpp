#pragma once

#include "database/database.hpp"
#include "database/databaseconnectionpool.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class DatabaseController {
public:
    DatabaseController(const std::shared_ptr<DatabaseConnectionPool>& dbConnPool);
    ~DatabaseController() = default;
    json executeQuery(const std::string& query);
    json executeReadQuery(const std::string& query);
    bool checkItemExists(const std::string& table, const std::string& column, const std::string& value);
    std::string getPasswordHashForUserID(const uint64_t& user_id, const std::string& tablename);
    uint64_t findIfUserID(const std::string& username, const std::string& tablename); // check if user found and return 0 if not found or return ID if found

private:
    std::shared_ptr<DatabaseConnectionPool> databaseConnectionPool;

    template <typename R, typename F, typename... Args>
    R executer(const F& f, Args&&... args)
    {
        std::shared_ptr<Database> db = nullptr;
        try {
            db = databaseConnectionPool->get_connection();
            R results = (db.get()->*f)(std::forward<Args>(args)...);
            databaseConnectionPool->return_connection(std::move(db));
            return results;
        } catch (const std::exception& e) {
            std::cerr << "Exception occurred during query execution: " << e.what() << std::endl;
            throw;
        } catch (...) {
            std::cerr << "Unknown exception occurred during query execution." << std::endl;
            throw;
        }
        return R();
    }
};
