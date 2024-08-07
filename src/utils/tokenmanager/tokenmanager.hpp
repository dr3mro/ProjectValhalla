#pragma once
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "store/types.hpp"
#include "utils/envvars/envvars.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include <jwt-cpp/jwt.h>
#include <optional>
/**
 * @class TokenManager
 * @brief Manages the generation and validation of JWT tokens for user authentication.
 *
 * The TokenManager class is responsible for generating and validating JWT tokens for user authentication. It uses the jwt-cpp library to handle the token creation and verification.
 *
 * The TokenManager class has the following main functions:
 * - `GenerateToken`: Generates a JWT token for a given user information.
 * - `ValidateToken`: Validates a JWT token and extracts the user information from it.
 *
 * The TokenManager class also has a `TokenManagerParameters` struct that holds the configuration parameters for the token management, such as the issuer, type, validity, and secret.
 *
 * The TokenManager class uses the `DatabaseController` and `SessionManager` classes to interact with the database and session management, respectively.
 */
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
        sessionManager = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(Type::SessionManager));
    }
    virtual ~TokenManager() = default;

    std::optional<std::string> GenerateToken(const LoggedUserInfo& loggedinUserInfo) const;
    bool ValidateToken(LoggedUserInfo& loggedinUserInfo) const;

private:
    TokenManagerParameters tokenMgrParm;
    std::shared_ptr<DatabaseController> databaseController;
    std::shared_ptr<SessionManager> sessionManager;
    jwt::verifier<jwt::default_clock, jwt::traits::kazuho_picojson> createTokenVerifier(const LoggedUserInfo& loggedinUserInfo) const;
    void fillUserInfo(LoggedUserInfo& loggedinUserInfo, const jwt::decoded_jwt<jwt::traits::kazuho_picojson>& token) const;
    bool validateUserInDatabase(const LoggedUserInfo& loggedinUserInfo) const;
};