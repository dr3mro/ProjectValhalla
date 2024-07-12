#pragma once

#include <jwt-cpp/jwt.h>

class Tokenizer {
public:
    typedef struct LoggedUserInfo {
        std::optional<std::string> token;
        std::optional<std::string> username;
        std::optional<uint64_t> user_id;
    } LoggedUserInfo;

    Tokenizer() = default;
    ~Tokenizer() = default;
    std::string generate_token(const std::string& username, const std::string& user_id);
    bool token_validator(LoggedUserInfo& userinfo);

private:
};