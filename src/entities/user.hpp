#pragma once
#include "entities/client.hpp"

class User : public Client {
public:
    User(const UserData& user_data, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt)
        : Client(user_data, databaseController, passwordCrypt, TABLENAME)
    {
    }
    User(const Credentials& credentials, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt)
        : Client(credentials, databaseController, passwordCrypt, TABLENAME)
    {
    }
    template <typename T>
    User(const T& data, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt)
        : Client(data, databaseController, passwordCrypt, TABLENAME)
    {
    }
    ~User() = default;

private:
    static constexpr auto TABLENAME = "users";
};