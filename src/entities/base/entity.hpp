#pragma once

#include "entities/base/base.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "jsoncons/basic_json.hpp"
#include "store/store.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include <any>
#include <regex>
using json = jsoncons::json;

/**
 * @brief The Entity class represents a base entity in the application, providing functionality for CRUD operations and search.
 *
 * The Entity class serves as a base class for various entities in the application, such as users, providers and services, etc. It provides a set of nested structs to handle different types of data related to the entity, including creation, reading, updating, deleting, and searching.
 *
 * The `UserData` struct represents the user-specific data, including username, password, email, role, and other optional fields. It also provides validation methods for the username, password, and email.
 *
 * The `Credentials` struct represents the user's login credentials, including the username and password.
 *
 * The `CreateData`, `ReadData`, `UpdateData`, `DeleteData`, and `SearchData` structs represent the data required for the corresponding CRUD operations and search functionality.
 *
 * The Entity class provides methods to generate SQL statements for these CRUD operations and search, which can be used by the application to interact with the database.
 */
class Entity : public Base {
public:
    /**
     * @brief The UserData struct represents the user-specific data, including username, password, email, role, and other optional fields. It provides validation methods for the username, password, and email.
     *
     * The `username`, `password`, `password_hash`, `email`, `role`, `basic_data`, and `service_data` fields are stored in the `db_data` vector, which can be used to interact with the database.
     *
     * The `validateUsername()`, `validatePassowrd()`, and `validateEmail()` methods provide validation for the corresponding fields, ensuring they meet the specified patterns.
     *
     * The `UserData` struct is constructed from a JSON object, which is used to populate the fields and generate the `db_data` vector.
     */

    using UserData = struct UserData {
    private:
        std::shared_ptr<PasswordCrypt> passwordCrypt = std::any_cast<std::shared_ptr<PasswordCrypt>>(Store::getObject(Type::PasswordCrypt));

    public:
        std::vector<std::pair<std::string, std::string>> db_data;

        json payload;

        std::optional<std::string> username;
        std::optional<std::string> password;
        std::optional<std::string> password_hash;
        std::optional<std::string> email;
        std::optional<std::string> role;
        std::optional<std::string> basic_data;
        std::optional<std::string> service_data;

        /**
         * @brief Validates the username according to the specified pattern.
         *
         * The username must start with a lowercase letter and can only contain lowercase letters, digits, and underscores.
         *
         * @return `true` if the username is valid, `false` otherwise.
         */

        bool validateUsername() const {
            const std::regex pattern("^[a-z][a-z0-9_]*$");
            return std::regex_match(username.value(), pattern);
        }
        /**
         * @brief Validates the password according to the specified pattern.
         *
         * The password must be at least 8 characters long and contain at least one lowercase letter, one uppercase letter, one digit, and one special character.
         *
         * @return `true` if the password is valid, `false` otherwise.
         */

        bool validatePassowrd() const {
            const std::regex pattern(
                "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*]"
                ")[A-Za-z\\d!@#$%^&*]{8,}$");
            return std::regex_match(password.value(), pattern);
        }
        /**
         * @brief Validates the email address according to a standard email pattern.
         *
         * The email must contain a username, followed by an optional period, followed by a domain name with at least one period.
         *
         * @return `true` if the email is valid, `false` otherwise.
         */

        bool validateEmail() const {
            const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
            return std::regex_match(email.value(), pattern);
        }

        UserData(const json &_data) {
            json response;
            try {
                // Extract the username from the JSON
                payload = _data["payload"];

                username = payload.at("username").as<std::string>();
                password = payload.at("password").as<std::string>();
                password_hash = passwordCrypt->hashPassword(password.value()).value();
                email = payload.at("basic_data").at("contact").at("email").as<std::string>();
                role = payload.at("basic_data").at("role").as<std::string>();
                basic_data = payload.at("basic_data").as<std::string>();

                if (payload.contains("service_data")) {
                    service_data = payload.at("service_data").as<std::string>();
                }

                db_data.push_back({ "username", username.value() });
                db_data.push_back({ "password_hash", password_hash.value() });
                db_data.push_back({ "role", role.value() });
                db_data.push_back({ "basic_data", basic_data.value() });

                if (service_data) {
                    db_data.push_back({ "service_data", service_data.value() });
                }

                payload.erase("password");

            } catch (const std::exception &e) {
                std::cerr << fmt::format("failed to create user: {}\n", e.what());
            }
        }
    };

    /**
     * @brief Represents a user's login credentials, including a username and password.
     */
    struct Credentials {
        std::string username; ///< The user's username.
        std::string password; ///< The user's password.
    };

    /**
     * @brief Represents data for creating a new entity.
     *
     * This struct contains the payload data and the next ID to be used for the new entity.
     */
    struct CreateData {
        json payload;        ///< The payload data for the new entity.
        uint64_t next_id;    ///< The next ID to be used for the new entity.

        /**
         * @brief Constructs a new CreateData instance.
         *
         * @param _payload The payload data for the new entity.
         * @param nid The next ID to be used for the new entity.
         */
        CreateData(const json &_payload, const uint64_t nid)
            : payload(_payload), next_id(nid) {}

        /**
         * @brief Default constructor for CreateData.
         */
        CreateData() = default;
    };

    /**
     * @brief Represents data for reading an entity.
     *
     * This struct contains the schema of the entity and the ID of the entity to be read.
     */
    struct ReadData {
        std::vector<std::string> schema; ///< The schema of the entity.
        uint64_t id;                    ///< The ID of the entity to be read.

        /**
         * @brief Constructs a new ReadData instance.
         *
         * @param _schema The schema of the entity.
         * @param _id The ID of the entity to be read.
         */
        ReadData(const std::vector<std::string> &_schema, const uint64_t _id)
            : schema(_schema), id(_id) {}

        /**
         * @brief Default constructor for ReadData.
         */
        ReadData() = default;
    };

    /**
     * @brief Represents data for updating an entity.
     *
     * This struct contains the payload data and the user ID associated with the update operation.
     */
    struct UpdateData {
        json payload;        ///< The payload data for the update.
        uint64_t user_id;    ///< The user ID associated with the update.

        /**
         * @brief Constructs a new UpdateData instance.
         *
         * @param _data The payload data for the update.
         * @param id The user ID associated with the update.
         */
        UpdateData(const json &_data, const uint64_t id)
            : payload(_data), user_id(id) {}

        /**
         * @brief Default constructor for UpdateData.
         */
        UpdateData() = default;
    };


    /**
     * @brief Represents data for deleting an entity.
     *
     * This struct contains the payload data and the user ID associated with the delete operation.
     *
     * @param payload The payload data for the delete operation.
     * @param user_id The user ID associated with the delete operation.
     */
    struct DeleteData {
        json payload;        ///< The payload data for the delete operation.
        uint64_t user_id;    ///< The user ID associated with the delete operation.

        /**
         * @brief Constructs a new DeleteData instance.
         *
         * @param _data The payload data for the delete operation.
         * @param id The user ID associated with the delete operation.
         */
        DeleteData(const json &_data, const uint64_t id)
            : payload(_data), user_id(id) {}

        /**
         * @brief Default constructor for DeleteData.
         */
        DeleteData() = default;
    };

    /**
     * @brief Represents search data for querying an entity.
     *
     * This struct contains the search parameters, such as the keyword, order by column, sort direction, limit, and offset, used to query an entity.
     *
     * @param keyword The search keyword.
     * @param order_by The column to order the results by.
     * @param direction The sort direction, either "ASC" or "DESC".
     * @param limit The maximum number of results to return.
     * @param offset The number of results to skip.
     */
    using SearchData = struct SearchData {
        std::string keyword;
        std::string order_by;
        std::string direction;
        size_t limit;
        size_t offset;

        SearchData(const json &search_json) {
            keyword = search_json.at("keyword").as<std::string>();
            order_by = search_json.at("order_by").as<std::string>();
            direction = search_json.at("direction").as<short>() == 0 ? "ASC" : "DESC";
            limit = search_json.at("limit").as<size_t>();
            offset = search_json.at("offset").as<size_t>();
        }
        SearchData() = default;
    };

    /**
     * @brief Represents data for logging out a user.
     *
     * This struct contains an optional token value associated with the logout operation.
     *
     * @param token The optional token value for the logout operation.
     */
    struct LogoutData {
        std::optional<std::string> token;
        LogoutData(const std::optional<std::string> &_token) {
            token = _token;
        }
    };


    template <typename T>
    Entity(const T &_data, const std::string &_tablename) : tablename(_tablename), data(_data) {
        passwordCrypt = std::any_cast<std::shared_ptr<PasswordCrypt>>(Store::getObject(Type::PasswordCrypt));
    }
    Entity(const std::string &_tablename) : tablename(_tablename) {
    }

    ~Entity() override = default;

    /**
     * Generates an SQL query for creating a new record in the database.
     *
     * This method is part of the `Entity` class, which is responsible for managing
     * database interactions for a specific table. The `getSqlCreateStatement()`
     * method generates an SQL `INSERT INTO` query based on the data provided in the
     * `CreateData` struct.
     *
     * The method extracts the payload and next_id from the `CreateData` struct,
     * iterates through the payload object, and constructs the SQL query with the
     * appropriate column names and values. If the payload contains an "id" field
     * in the "basic_data" object, the method updates the "id" value with the
     * provided `next_id`.
     *
     * The generated SQL query is returned as an optional string, or `std::nullopt`
     * if an exception occurs during the query construction.
     *
     * @return An optional string containing the generated SQL query, or `std::nullopt`
     *         if an exception occurs.
     */
    std::optional<std::string> getSqlCreateStatement() override {
        std::optional<std::string> query;
        try {
            std::vector<std::string> keys_arr;
            std::vector<std::string> values_arr;

            json payload = std::any_cast<CreateData>(data).payload;
            uint64_t next_id = std::any_cast<CreateData>(data).next_id;
            for (auto &it : payload.object_range()) {
                if (it.key() == "basic_data" && it.value().contains("id")) {
                    it.value()["id"] = next_id;
                }

                keys_arr.push_back(it.key());
                values_arr.push_back(it.value().as<std::string>());
            }

            std::string columns = fmt::format("{}", fmt::join(keys_arr, ","));
            std::string values = fmt::format("'{}'", fmt::join(values_arr, "','"));

            query = fmt::format("INSERT INTO {} (id, {}) VALUES ({},{}) RETURNING id,{};", tablename, columns, next_id, values, columns);

        } catch (const std::exception &e) {
            std::cerr << "faild to create query for create " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    };
    /**
     * Generates an SQL query for reading a record from the database.
     *
     * This method is part of the `Entity` class, which is responsible for managing
     * database interactions for a specific table. The `getSqlReadStatement()`
     * method generates an SQL `SELECT` query based on the data provided in the
     * `ReadData` struct.
     *
     * The method extracts the `id` and `schema` from the `ReadData` struct, and
     * constructs the SQL query with the appropriate column names and the `id`
     * value as the filter. The generated SQL query is returned as an optional
     * string, or `std::nullopt` if an exception occurs during the query
     * construction.
     *
     * @return An optional string containing the generated SQL query, or `std::nullopt`
     *         if an exception occurs.
     */
    std::optional<std::string> getSqlReadStatement() override {
        std::optional<std::string> query;
        try {
            auto user_id = std::any_cast<ReadData>(data).id;
            auto schema = std::any_cast<ReadData>(data).schema;

            std::string columns = fmt::format("{}", fmt::join(schema, ", "));

            query = fmt::format("SELECT {} FROM {} WHERE id={} LIMIT 1;", columns, tablename, user_id);
        } catch (const std::exception &e) {
            std::cerr << "faild to create query for read " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }
    /**
     * Generates an SQL query for updating a record in the database.
     *
     * This method is part of the `Entity` class, which is responsible for managing
     * database interactions for a specific table. The `getSqlUpdateStatement()`
     * method generates an SQL `UPDATE` query based on the data provided in the
     * `UpdateData` struct.
     *
     * The method extracts the `payload` and `user_id` from the `UpdateData` struct,
     * and constructs the SQL query with the appropriate column names and values to
     * be updated. The generated SQL query is returned as an optional string, or
     * `std::nullopt` if an exception occurs during the query construction.
     *
     * @return An optional string containing the generated SQL query, or `std::nullopt`
     *         if an exception occurs.
     */
    std::optional<std::string> getSqlUpdateStatement() override {
        std::optional<std::string> query;

        try {
            json payload = std::any_cast<UpdateData>(data).payload;

            uint64_t id = std::any_cast<UpdateData>(data).user_id;
            ;

            std::string update_column_values;

            for (auto it = payload.object_range().begin(); it != payload.object_range().end(); ++it) {
                update_column_values.append(fmt::format(" {} = '{}' ", it->key(), it->value().as<std::string>()));
                if (std::next(it) != payload.object_range().end()) {
                    update_column_values.append(",");
                }
            }

            query = fmt::format("UPDATE {} set {} WHERE id={};", tablename, update_column_values, id);
        } catch (const std::exception &e) {
            std::cerr << "faild to create query for update " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }

    /**
     * Generates an SQL query for deleting a record from the database.
     *
     * This method is part of the `Entity` class, which is responsible for managing
     * database interactions for a specific table. The `getSqlDeleteStatement()`
     * method generates an SQL `DELETE` query based on the data provided in the
     * `DeleteData` struct.
     *
     * The method extracts the `id` from the `DeleteData` struct, and constructs the
     * SQL query to delete the record with the specified `id` from the table. The
     * generated SQL query is returned as an optional string, or `std::nullopt` if
     * an exception occurs during the query construction.
     *
     * @return An optional string containing the generated SQL query, or `std::nullopt`
     *         if an exception occurs.
     */
    std::optional<std::string> getSqlDeleteStatement() override {
        std::optional<std::string> query;
        uint64_t id;

        try {
            jsoncons::json payload = std::any_cast<DeleteData>(data).payload;
            jsoncons::json basic_data = payload.at("basic_data");
            id = basic_data.at("id").as<uint64_t>();

            // Construct SQL query using {fmt} for parameterized query
            query = fmt::format("DELETE FROM {} where id={};", tablename, id);

        } catch (const std::exception &e) {
            std::cerr << "faild to create query for delete " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }
    /**
     * Generates an SQL query for searching records in the database.
     *
     * This method is part of the `Entity` class, which is responsible for managing
     * database interactions for a specific table. The `getSqlSearchStatement()`
     * method generates an SQL `SELECT` query based on the data provided in the
     * `SearchData` struct.
     *
     * The method extracts the search parameters (keyword, order_by, direction, limit,
     * offset) from the `SearchData` struct, and constructs the SQL query to search
     * for records in the table that match the provided criteria. The generated SQL
     * query is returned as an optional string, or `std::nullopt` if an exception
     * occurs during the query construction.
     *
     * @return An optional string containing the generated SQL query, or `std::nullopt`
     *         if an exception occurs.
     */
    std::optional<std::string> getSqlSearchStatement() override {
        std::optional<std::string> query;
        try {
            SearchData searchdata = std::any_cast<SearchData>(data);
            query = fmt::format(
                "SELECT basic_data FROM {} WHERE basic_data::text "
                "ILIKE '%{}%' ORDER BY {} {} LIMIT {} OFFSET {};",
                tablename,
                searchdata.keyword,
                searchdata.order_by,
                searchdata.direction,
                searchdata.limit + 1,
                searchdata.offset);

        } catch (const std::exception &e) {
            std::cerr << "faild to create query for search " << tablename << e.what() << std::endl;
            return std::nullopt;
        }

        return query;
    }
    /**
     * Gets the data associated with this entity.
     *
     * This method returns the data associated with the current entity instance. The
     * data is stored in a `std::any` object, which can hold any type of data.
     *
     * @return The data associated with this entity.
     */
    std::any getData() const {
        return data;
    }
    /**
     * Gets the name of the table associated with this entity.
     *
     * This method returns the name of the database table that this entity represents.
     *
     * @return The name of the table associated with this entity.
     */
    std::string getGroupName() const // ie. tablename
    {
        return tablename;
    }

protected:
    std::string tablename;

private:
    std::any data;
    std::shared_ptr<PasswordCrypt> passwordCrypt;
};
