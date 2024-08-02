#pragma once

#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/entity.hpp"
#include "fmt/format.h"
#include "store/store.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include <jsoncons/json.hpp>
#include <memory>
#include <utility>

#define USERNAME "username"

using json
    = jsoncons::json;

class Client : public Entity {
public:
    template <typename T>
    Client(const T& data, const std::string& tablename)
        : Entity(data, tablename)

    {
        databaseController = std::any_cast<std::shared_ptr<DatabaseController>>(Store::getObject(Type::DatabaseController));
        passwordCrypt = std::any_cast<std::shared_ptr<PasswordCrypt>>(Store::getObject(Type::PasswordCrypt));
    }
    std::optional<std::string> getSqlCreateStatement() override
    {
        auto userdata = std::any_cast<Entity::UserData>(getData());
        return fmt::format(
            "INSERT INTO {} (username, password_hash, role, basic_data) VALUES ('{}','{}','{}','{}')",
            tablename, userdata.username, userdata.password_hash, userdata.role, userdata.user_data);
    }

    std::optional<bool> exists(const std::string& username)
    {
        return databaseController->checkItemExists(tablename, USERNAME, username);
    }

    std::optional<uint64_t> authenticate() const
    {
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

        } catch (const std::exception& e) {
            std::cerr << "Error authenticating user : " << e.what() << std::endl;
        }
        return std::nullopt;
    }
    std::pair<bool, std::string> validate()
    {
        auto userdata = std::any_cast<Entity::UserData>(getData());
        std::string username = userdata.username;

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
    virtual ~Client() = default;

private:
    std::shared_ptr<DatabaseController> databaseController;
    std::shared_ptr<PasswordCrypt> passwordCrypt;
};