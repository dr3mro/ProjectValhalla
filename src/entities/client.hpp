#pragma once

#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/entity.hpp"
#include "fmt/format.h"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include <jsoncons/json.hpp>
#include <memory>
#include <utility>

#define USERNAME "username"

using json
    = jsoncons::json;

class Client : public Entity {
public:
    Client(const UserData& user_data, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt, const std::string& tablename)
        : Entity(user_data, tablename)
        , databaseController(databaseController)
        , passwordCrypt(passwordCrypt)

    {
    }
    Client(const Credentials& credentials, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt, const std::string& tablename)
        : Entity(credentials, tablename)
        , databaseController(databaseController)
        , passwordCrypt(passwordCrypt)
    {
    }
    template <typename T>
    Client(const T& data, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt, const std::string& tablename)
        : Entity(data, tablename)
        , databaseController(databaseController)
        , passwordCrypt(passwordCrypt)

    {
    }
    std::optional<std::string> getSqlCreateStatement() override
    {
        auto userdata = std::any_cast<Entity::UserData>(getData());
        return fmt::format(
            "INSERT INTO {} (username, password_hash, role, basic_data) VALUES ('{}','{}','{}','{}')",
            tablename, userdata.username, userdata.password_hash, userdata.role, userdata.user_data);
    }

    bool exists(const std::string& username)
    {
        return databaseController->checkItemExists(tablename, USERNAME, username);
    }

    uint64_t authenticate() const
    {
        try {
            auto userpass = std::any_cast<Entity::Credentials>(getData());
            uint64_t user_id = databaseController->findIfUserID(std::cref(userpass.username), std::cref(tablename));

            if (user_id == 0)
                return 0;

            if (passwordCrypt->verifyPassword(userpass.password, databaseController->getPasswordHashForUserID(std::cref(user_id), std::cref(tablename))))
                return user_id;
            else
                return 0;

        } catch (const std::exception& e) {
            std::cerr << "Error authenticating user : " << e.what() << std::endl;
        }
        return 0;
    }
    std::pair<bool, std::string> validate()
    {
        auto userdata = std::any_cast<Entity::UserData>(getData());
        std::string username = userdata.username;

        if (exists(username)) {
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