#pragma once

#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/base/entity.hpp"
#include "fmt/format.h"
#include "store/store.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include <jsoncons/json.hpp>
#include <memory>
#include <utility>

#define USERNAME "username"

using json = jsoncons::json;

/// Represents a client entity in the application. The `Client` class inherits from the `Entity` class and provides functionality for managing client-related data, including authentication, validation, and database operations.
///
/// The `Client` class has the following key features:
/// - Retrieves the `DatabaseController` and `PasswordCrypt` instances from the `Store` to handle database operations and password encryption/decryption.
/// - Provides a `getSqlCreateStatement()` method to generate an SQL query for creating a new client record in the database.
/// - Implements an `exists()` method to check if a client with a given username already exists in the database.
/// - Provides an `authenticate()` method to authenticate a client using their username and password.
/// - Implements a `validate()` method to validate the client's data, including username, password, and email.
/// Represents a client entity in the application. The `Client` class inherits from the `Entity` class and provides functionality for managing client-related data, including authentication, validation, and database operations.
///
/// The `Client` class has the following key features:
/// - Retrieves the `DatabaseController` and `PasswordCrypt` instances from the `Store` to handle database operations and password encryption/decryption.
/// - Provides a `getSqlCreateStatement()` method to generate an SQL query for creating a new client record in the database.
/// - Implements an `exists()` method to check if a client with a given username already exists in the database.
/// - Provides an `authenticate()` method to authenticate a client using their username and password.
/// - Implements a `validate()` method to validate the client's data, including username, password, and email.
class Client : public Entity {
public:
    template <typename T>
    /// Constructs a new `Client` object with the given data and table name.
    ///
    /// @param data The data to initialize the `Client` object with.
    /// @param tablename The name of the database table associated with the `Client` object.
    Client(const T &data, const std::string &tablename)
        : Entity(data, tablename)

    {
        try {
            databaseController = std::any_cast<std::shared_ptr<DatabaseController>>(Store::getObject(Type::DatabaseController));
            passwordCrypt = std::any_cast<std::shared_ptr<PasswordCrypt>>(Store::getObject(Type::PasswordCrypt));
        } catch (const std::exception &e) {
            std::cerr << "Exception caught in Client constructor: " << e.what() << std::endl;
            EXIT_FAILURE;
        }
    }
    /// Generates an SQL query for creating a new client record in the database.
    ///
    /// @return An optional string containing the SQL query, or `std::nullopt` if an exception occurred.
    std::optional<std::string> getSqlCreateStatement() override {
        auto userdata = std::any_cast<Entity::UserData>(getData());

        try {
            std::vector<std::string> keys_arr;
            std::vector<std::string> values_arr;
            for (auto &it : userdata.db_data) {
                keys_arr.push_back(it.first);
                values_arr.push_back(it.second);
            }

            std::string columns = fmt::format("{}", fmt::join(keys_arr, ","));
            std::string values = fmt::format("'{}'", fmt::join(values_arr, "','"));

            return fmt::format("INSERT INTO {} ({}) VALUES ({}) RETURNING id;", tablename, columns, values);

        } catch (const std::exception &e) {
            std::cerr << "faild to create query for create " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return std::nullopt;
    }

    /// Checks if a client with the given username already exists in the database.
    ///
    /// @param username The username to check for.
    /// @return An optional boolean indicating whether the client exists (true) or not (false), or `std::nullopt` if an error occurred.
    std::optional<bool> exists(const std::string &username) {
        return databaseController->checkItemExists(tablename, USERNAME, username);
    }

    /// Authenticates the client by checking the provided username and password against the stored credentials.
    ///
    /// @return An optional `uint64_t` containing the client ID if the authentication is successful, or `std::nullopt` if the authentication fails or an error occurs.
    std::optional<uint64_t> authenticate() const {
        try {
            auto credentials = std::any_cast<Entity::Credentials>(getData());
            auto client_id = databaseController->findIfUserID(std::cref(credentials.username), std::cref(tablename));

            if (!client_id)
                return std::nullopt;

            auto hash = databaseController->getPasswordHashForUserID(std::cref(client_id.value()), std::cref(tablename));

            if (!hash) {
                return std::nullopt;
            }

            if (passwordCrypt->verifyPassword(credentials.password, hash.value())) {
                return client_id;
            }

        } catch (const std::exception &e) {
            std::cerr << "Error authenticating user : " << e.what() << std::endl;
        }
        return std::nullopt;
    }
    /// Validates the client's user data, including the username, password, and email.
    ///
    /// @return A pair containing a boolean indicating whether the validation was successful, and an optional error message if the validation failed.

    std::pair<bool, std::string> validate() {
        auto userdata = std::any_cast<Entity::UserData>(getData());
        std::string username = userdata.username.value();

        if (exists(username).value()) {
            return { false, fmt::format("username already exists in {}.", tablename) };
        } else if (!userdata.validateUsername()) {
            return { false, "username is invalid" };
        } else if (!userdata.validatePassowrd()) {
            return { false, "password is invalid" };
        } else if (!userdata.validateEmail()) {
            return { false, "email format is invalid" };
        }
        return { true, "validation success" };
    }
    ~Client() override = default;

private:
    std::shared_ptr<DatabaseController> databaseController;
    std::shared_ptr<PasswordCrypt> passwordCrypt;
};