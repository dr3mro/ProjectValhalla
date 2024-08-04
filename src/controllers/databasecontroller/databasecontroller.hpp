#pragma once
#include "database/database.hpp"
#include "database/databaseconnectionpool.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class DatabaseController {
public:
    DatabaseController();
    ~DatabaseController() = default;

    std::optional<json> executeQuery(const std::string &query);
    std::optional<json> executeReadQuery(const std::string &query);
    std::optional<std::string> doReadQuery(const std::string &query);
    std::optional<bool> checkItemExists(const std::string &table, const std::string &column, const std::string &value);
    std::optional<std::string> getPasswordHashForUserID(const uint64_t &user_id, const std::string &tablename);
    std::optional<uint64_t> findIfUserID(
        const std::string &username,
        const std::string &tablename); // check if user found and return 0 if not
                                       // found or return ID if found

private:
    std::shared_ptr<DatabaseConnectionPool> databaseConnectionPool;

    template <typename R, typename F, typename... Args>
    std::optional<R> executer(const F &f, Args &&...args) {
        std::shared_ptr<Database> db = nullptr;
        try {
            db = databaseConnectionPool->get_connection();
            std::optional<R> results = std::invoke(f, db.get(), std::forward<Args>(args)...);
            //(db.get()->*f)(std::forward<Args>(args)...);
            databaseConnectionPool->return_connection(std::move(db));
            if (results) {
                return results.value();
            }
        } catch (const std::exception &e) {
            std::cerr << "Exception occurred during query execution: " << e.what() << std::endl;
            throw;
        } catch (...) {
            std::cerr << "Unknown exception occurred during query execution." << std::endl;
            throw;
        }
        return std::nullopt;
    }
};
