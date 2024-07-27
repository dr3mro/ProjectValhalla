#include "tokenmanager.hpp"

// Function to generate JWT token
using jwt::error::token_verification_exception;

std::optional<std::string>
TokenManager::GenerateToken(const LoggedUserInfo& loggedinUserInfo) const
{
    try {
        // Create JWT token with payload
        std::optional<std::string> token = jwt::create()
                                               .set_issuer(tokenMgrParm.issuer)
                                               .set_type(tokenMgrParm.type)
                                               .set_subject(loggedinUserInfo.userName.value())
                                               .set_id(std::to_string(loggedinUserInfo.userID.value()))
                                               .set_issued_at(std::chrono::system_clock::now())
                                               .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes { std::stoull(tokenMgrParm.validity) })
                                               .set_payload_claim("llod", jwt::claim(loggedinUserInfo.llod.value()))
                                               .set_payload_claim("group", jwt::claim(loggedinUserInfo.group.value()))
                                               .sign(jwt::algorithm::hs256 { tokenMgrParm.secret });

        return token;
    } catch (const std::exception& e) {
        std::cerr << "Error generating token: " << e.what() << std::endl;
    }
    return std::nullopt;
}

bool TokenManager::ValidateToken(LoggedUserInfo& loggedinUserInfo) const
{
    try {
        // Create a verifier
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256 { tokenMgrParm.secret })
                            .with_issuer(tokenMgrParm.issuer)
                            .with_type(tokenMgrParm.type);

        // Decode the token
        auto decoded_token = jwt::decode(loggedinUserInfo.token.value());

        // Verify the token
        verifier.verify(decoded_token);

        // Set user info
        loggedinUserInfo.userID = std::stoull(decoded_token.get_id());
        loggedinUserInfo.userName = decoded_token.get_subject();
        loggedinUserInfo.group = decoded_token.get_payload_claim("group").as_string();
        loggedinUserInfo.llod = decoded_token.get_payload_claim("llod").as_string();

        // Check expiration
        auto exp_time = decoded_token.get_payload_claim("exp").as_date().time_since_epoch();
        auto issuer = decoded_token.get_payload_claim("iss").as_string();
        auto now = std::chrono::system_clock::now().time_since_epoch();

        // Check if token is still valid
        if (now < exp_time && issuer == tokenMgrParm.issuer) {
            return true;
        }

    } catch (const token_verification_exception& e) {
        std::cerr << "Token verification failed: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error validating token: " << e.what() << std::endl;
    }
    return false;
}