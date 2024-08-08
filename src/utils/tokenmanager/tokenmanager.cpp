#include "tokenmanager.hpp"


// Function to generate JWT token
using jwt::error::token_verification_exception;

/**
 * Generates a JWT token with the provided user information.
 *
 * @param loggedinUserInfo The user information to include in the token.
 * @return An optional string containing the generated JWT token, or std::nullopt if an error occurred.
 */
std::optional<std::string> TokenManager::GenerateToken(const LoggedUserInfo &loggedinUserInfo) const {
    try {
        // Create JWT token with payload
        std::optional<std::string> token = jwt::create()
                                               .set_issuer(tokenMgrParm.issuer)
                                               .set_type(tokenMgrParm.type)
                                               .set_subject(loggedinUserInfo.userName.value())
                                               .set_id(std::to_string(loggedinUserInfo.userID.value()))
                                               .set_issued_at(std::chrono::system_clock::now())
                                               .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes{ std::stoull(tokenMgrParm.validity) })
                                               .set_payload_claim("llodt", jwt::claim(loggedinUserInfo.llodt.value()))
                                               .set_payload_claim("group", jwt::claim(loggedinUserInfo.group.value()))
                                               .sign(jwt::algorithm::hs256{ tokenMgrParm.secret });

        return token;
    } catch (const std::exception &e) {
        std::cerr << "Error generating token: " << e.what() << std::endl;
    }
    return std::nullopt;
}

/**
 * Validates the provided JWT token and updates the user information from the token.
 *
 * @param loggedinUserInfo The user information, including the token to be validated.
 * @return `true` if the token is valid, `false` otherwise.
 */
bool TokenManager::ValidateToken(LoggedUserInfo &loggedinUserInfo) const {
    try {
        auto token = jwt::decode(loggedinUserInfo.token.value());

        // Validate token expiration
        auto now = std::chrono::system_clock::now().time_since_epoch();
        auto exp_time = token.get_payload_claim("exp").as_date().time_since_epoch();
        if (now >= exp_time) {
            return false;
        }

        // Update user info from token
        fillUserInfo(loggedinUserInfo, token);

        // Validate token claims
        auto verifier = createTokenVerifier(loggedinUserInfo);
        verifier.verify(token);


        // Validate user in database
        if (!validateUserInDatabase(loggedinUserInfo)) {
            return false;
        }


        return true;
    } catch (const token_verification_exception &e) {
        std::cerr << "Token verification failed: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error validating token: " << e.what() << std::endl;
    }
    return false;
}

/**
 * Creates a JWT verifier for validating the provided token.
 *
 * @param loggedinUserInfo The logged-in user information, including the token to be verified.
 * @return A JWT verifier that can be used to verify the token.
 */
jwt::verifier<jwt::default_clock, jwt::traits::kazuho_picojson> TokenManager::createTokenVerifier(const LoggedUserInfo &loggedinUserInfo) const {
    return jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{ tokenMgrParm.secret })
        .with_issuer(tokenMgrParm.issuer)
        .with_type(tokenMgrParm.type)
        .with_subject(loggedinUserInfo.userName.value())
        .with_id(std::to_string(loggedinUserInfo.userID.value()))
        .with_claim("group", jwt::claim(loggedinUserInfo.group.value()))
        .with_claim("llodt", jwt::claim(loggedinUserInfo.llodt.value_or("first_login")));
}

/**
 * Fills the user information in the provided LoggedUserInfo object using the claims from the decoded JWT token.
 *
 * @param loggedinUserInfo The LoggedUserInfo object to be filled with user information.
 * @param token The decoded JWT token containing the user information.
 * @throws std::runtime_error if the required user information is missing from the token or if the group claim does not match the existing group in the
 * LoggedUserInfo object.
 */
void TokenManager::fillUserInfo(LoggedUserInfo &loggedinUserInfo, const jwt::decoded_jwt<jwt::traits::kazuho_picojson> &token) const {
    if (!loggedinUserInfo.group) {
        loggedinUserInfo.group = token.get_payload_claim("group").as_string();
    } else if (loggedinUserInfo.group != token.get_payload_claim("group").as_string()) {
        throw std::runtime_error("Group mismatch in token");
    }

    loggedinUserInfo.userID = std::stoull(token.get_id());
    loggedinUserInfo.userName = token.get_subject();

    if (!loggedinUserInfo.group || !loggedinUserInfo.userID || !loggedinUserInfo.userName) {
        throw std::runtime_error("Missing required user information in token");
    }
    // Get last logout time
    loggedinUserInfo.llodt = sessionManager->getLastLogoutTime(loggedinUserInfo.userID.value(), loggedinUserInfo.group.value());
}

/**
 * Validates if the provided user information matches the user in the database.
 *
 * @param loggedinUserInfo The logged-in user information to be validated.
 * @return True if the user information matches the database, false otherwise.
 */
bool TokenManager::validateUserInDatabase(const LoggedUserInfo &loggedinUserInfo) const {
    return databaseController->findIfUserID(loggedinUserInfo.userName.value(), loggedinUserInfo.group.value()) == loggedinUserInfo.userID.value();
}
