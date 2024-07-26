#pragma once
#include "entities/client.hpp"

class Provider : public Client {

public:
    Provider(const UserData& user_data, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt)
        : Client(user_data, databaseController, passwordCrypt, TABLENAME)
    {
    }
    Provider(const Credentials& credentials, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt)
        : Client(credentials, databaseController, passwordCrypt, TABLENAME)
    {
    }
    template <typename T>
    Provider(const T& data, const std::shared_ptr<DatabaseController>& databaseController, const std::shared_ptr<PasswordCrypt>& passwordCrypt)
        : Client(data, databaseController, passwordCrypt, TABLENAME)
    {
    }
    ~Provider() = default;

private:
    static constexpr auto TABLENAME = "providers";
};