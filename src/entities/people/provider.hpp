#pragma once
#include "entities/base/client.hpp"

class Provider : public Client {

public:
    Provider(const UserData &user_data) : Client(user_data, TABLENAME) {
    }
    Provider(const Credentials &credentials) : Client(credentials, TABLENAME) {
    }
    template <typename T>
    Provider(const T &data) : Client(data, TABLENAME) {
    }
    ~Provider() = default;

private:
    static constexpr auto TABLENAME = "providers";
};