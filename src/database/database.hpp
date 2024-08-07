#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <jsoncons/json.hpp>
#include <pqxx/pqxx> // Include the libpqxx header for PostgreSQL

using json = jsoncons::json;

/**
 * @class Database
 * @brief Provides an interface for interacting with a PostgreSQL database.
 *
 * The `Database` class is responsible for managing the connection to a PostgreSQL
 * database and executing SQL queries. It provides methods for checking if the
 * connection is established, checking if a table and column combination exists,
 * and executing both simple and complex queries.
 *
 * The `executeQuery` method allows executing arbitrary SQL queries and returns
 * the results as a JSON array. The `doSimpleQuery` method is a convenience
 * function for executing simple queries that return a single value.
 */
/**
 * @class Database
 * @brief Provides an interface for interacting with a PostgreSQL database.
 *
 * The `Database` class is responsible for managing the connection to a PostgreSQL
 * database and executing SQL queries. It provides methods for checking if the
 * connection is established, checking if a table and column combination exists,
 * and executing both simple and complex queries.
 *
 * The `executeQuery` method allows executing arbitrary SQL queries and returns
 * the results as a JSON array. The `doSimpleQuery` method is a convenience
 * function for executing simple queries that return a single value.
 */
/**
 * @class Database
 * @brief Provides an interface for interacting with a PostgreSQL database.
 *
 * The `Database` class is responsible for managing the connection to a PostgreSQL
 * database and executing SQL queries. It provides methods for checking if the
 * connection is established, checking if a table and column combination exists,
 * and executing both simple and complex queries.
 *
 * The `executeQuery` method allows executing arbitrary SQL queries and returns
 * the results as a JSON array. The `doSimpleQuery` method is a convenience
 * function for executing simple queries that return a single value.
 */
class Database {
public:
    Database(std::shared_ptr<pqxx::connection> conn);
    ~Database() = default;

    bool isConnected();
    bool checkExists(const std::string &table, const std::string &column, const std::string &value);

    template <typename TransactionType>
    json executeQuery(const std::string &query) {
        try {
            TransactionType txn(*connection);
            pqxx::result res = txn.exec(query);

            if constexpr (std::is_same_v<TransactionType, pqxx::work>) {
                txn.commit();
            }

            json json_array = json::array();
            json affected_rows;

            if constexpr (std::is_same_v<TransactionType, pqxx::work>) {
                affected_rows["affected rows"] = res.affected_rows();
                json_array.push_back(affected_rows);
            }

            for (const auto &row : res) {
                json jsonObj;
                for (const auto &field : row) {
                    jsonObj[field.name()] = json::parse(field.as<std::string>());
                }
                json_array.push_back(jsonObj);
            }

            return json_array;
        } catch (const std::exception &e) {
            std::cerr << "Error executing query: " << e.what() << std::endl;
            throw; // Rethrow the exception to indicate failure
        }
    }

    template <typename T>
    std::optional<T> doSimpleQuery(const std::string &query) {
        try {
            pqxx::nontransaction txn(*connection);
            pqxx::result result = txn.exec(query);
            return result[0][0].as<std::optional<T>>();
        } catch (const std::exception &e) {
            std::cerr << "Error executing query: " << e.what() << std::endl;
            throw; // Rethrow the exception to indicate failure
        }
    }

private:
    std::shared_ptr<pqxx::connection> connection;
};

#endif // DATABASE_HPP
