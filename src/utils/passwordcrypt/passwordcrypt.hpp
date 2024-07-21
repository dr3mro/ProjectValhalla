#pragma once
#include <optional>
#include <string>

class PasswordCrypt {
public:
    PasswordCrypt();
    ~PasswordCrypt() = default;

    std::optional<std::string> hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash) const;
};