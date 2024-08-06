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

class Client : public Entity {
public:
    template <typename T>
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

    std::optional<bool> exists(const std::string &username) {
        return databaseController->checkItemExists(tablename, USERNAME, username);
    }

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