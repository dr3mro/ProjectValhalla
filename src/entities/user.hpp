#pragma once

#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/entity.hpp"
#include "fmt/format.h"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include <jsoncons/json.hpp>
#include <memory>
#include <regex>
#include <utility>

using json = jsoncons::json;

class User : public Entity {
public:
    using UserData = struct UserData {
        std::string username;
        std::string password;
        std::string password_hash;
        std::string email;
        std::string role;
        std::string user_data;

        bool validateUsername() const
        {
            const std::regex pattern("^[a-z][a-z0-9_]*$");
            return std::regex_match(username, pattern);
        }
        bool validatePassowrd() const
        {
            const std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");
            return std::regex_match(password, pattern);
        }
        bool validateEmail() const
        {
            const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
            return std::regex_match(email, pattern);
        }
    };

    User(const json& user_data, const std::shared_ptr<DatabaseController>& databaseController)
        : databaseController(databaseController)
    {
        json response;
        try {
            // Extract the username from the JSON
            json payload = user_data["payload"];

            userdata.username = payload["username"].as<std::string>();
            userdata.password = payload["password"].as<std::string>();
            userdata.password_hash = passwordCrypt.hashPassword(userdata.password).value();
            userdata.email = payload["user_data"]["contact"]["email"].as<std::string>();
            userdata.role = payload["role"].as<std::string>();
            userdata.user_data = payload["user_data"].as<std::string>();

            payload.erase("password");

        } catch (const std::exception& e) {
            std::cerr << "failed to create user\n";
        }
    }

    User(const std::pair<std::string, std::string>& userpass, const std::shared_ptr<DatabaseController>& databaseController)
        : userpass(userpass)
        , databaseController(databaseController)
    {
    }

    bool exists()
    {
        return databaseController->checkItemExists("users", "username", userdata.username);
    }

    std::pair<bool, std::string> validate()
    {
        if (exists()) {
            return { false, "A user already exists with the same username." };
        } else if (!userdata.validateUsername()) {
            return { false, "username is invalid" };
        } else if (!userdata.validatePassowrd()) {
            return { false, "password is invalid" };
        } else if (!userdata.validateEmail()) {
            return { false, "email format is invalid" };
        }
        return { true, "validation success" };
    }

    std::optional<std::string> getSqlCreateStatement() override
    {
        return fmt::format(
            "INSERT INTO users (username, password_hash, role, user_data) VALUES ('{}','{}','{}','{}')",
            userdata.username, userdata.password_hash, userdata.role, userdata.user_data);
    }

    std::optional<std::string> getSqlReadStatement() override
    {
        return std::string();
    }

    std::optional<std::string> getSqlUpdateStatement() override
    {
        return std::string();
    }

    std::optional<std::string> getSqlDeleteStatement() override
    {
        return std::string();
    }

    std::optional<std::string> getSqlSearchStatement() override
    {
        return std::string();
    }

    uint64_t authenticate() const
    {
        try {
            uint64_t user_id = databaseController->findIfUserID(std::ref(userpass.first));

            if (user_id == 0)
                return 0;

            if (passwordCrypt.verifyPassword(userpass.second, databaseController->getPasswordHashForUserID(user_id)))
                return user_id;
            else
                return 0;

        } catch (const std::exception& e) {
            std::cerr << "Error authenticating user : " << e.what() << std::endl;
        }
        return 0;
    }

    ~User() override
    {
    }

private:
    UserData userdata;
    PasswordCrypt passwordCrypt;
    const std::pair<std::string, std::string> userpass;
    std::shared_ptr<DatabaseController> databaseController;
};