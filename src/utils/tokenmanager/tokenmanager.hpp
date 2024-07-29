#pragma once
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "store/types.hpp"
#include "utils/envvars/envvars.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include <jwt-cpp/jwt.h>
#include <optional>
class TokenManager {
public:
    using LoggedUserInfo = struct LoggedUserInfo {
        std::optional<std::string> token;
        std::optional<std::string> userName;
        std::optional<std::string> group;
        std::optional<uint64_t> userID;
        std::optional<std::string> llodt; // used to invalidate tokens on logout
    };

    using TokenManagerParameters = struct TokenManagerParameters {
        std::string issuer;
        std::string type;
        std::string validity;
        std::string secret;

        TokenManagerParameters()
        {
            EnvVars evs;
            issuer = evs.get("JWT_ISSUER").value_or("valhalla");
            type = evs.get("JWT_TYPE").value_or("JWS");
            validity = evs.get("JWT_VALIDITY").value_or("43200");
            secret = evs.get("JWT_SECRET").value_or("01234567890123456789012345678901");
        }
    };
    TokenManager()
    {
        databaseController = std::any_cast<std::shared_ptr<DatabaseController>>(Store::getObject(Type::DatabaseController));
    }
    virtual ~TokenManager() = default;

    std::optional<std::string> GenerateToken(const LoggedUserInfo& loggedinUserInfo) const;
    bool ValidateToken(LoggedUserInfo& loggedinUserInfo) const;

protected:
    std::shared_ptr<SessionManager> sessionManager;

private:
    TokenManagerParameters tokenMgrParm;
    std::shared_ptr<DatabaseController> databaseController;
};