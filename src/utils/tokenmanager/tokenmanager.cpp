#include "tokenmanager.hpp"
#include "store/store.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"

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
                                               .set_payload_claim("llodt", jwt::claim(loggedinUserInfo.llodt.value()))
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

        auto token = jwt::decode(loggedinUserInfo.token.value());

        loggedinUserInfo.group = token.get_payload_claim("group").as_string();
        loggedinUserInfo.userID = std::stoull(token.get_id());
        loggedinUserInfo.userName = token.get_subject();

        if (!loggedinUserInfo.group || !loggedinUserInfo.userID || !loggedinUserInfo.userName) {
            return false;
        }

        Type sm_type;
        if (loggedinUserInfo.group == "users") {
            sm_type = Type::UserSessionManager;
        } else if (loggedinUserInfo.group == "providers") {
            sm_type = Type::ProviderSessionManager;
        } else {
            return false;
        }

        auto sm = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(sm_type));
        auto db = std::any_cast<std::shared_ptr<DatabaseController>>(Store::getObject(Type::DatabaseController));

        if (db->findIfUserID(loggedinUserInfo.userName.value(), loggedinUserInfo.group.value()) != loggedinUserInfo.userID) {
            return false;
        }

        loggedinUserInfo.llodt = sm->getLastLogoutTime(loggedinUserInfo.userID.value());

        // Check expiration
        auto exp_time = token.get_payload_claim("exp").as_date().time_since_epoch();
        auto issuer = token.get_payload_claim("iss").as_string();
        auto now = std::chrono::system_clock::now().time_since_epoch();

        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256 { tokenMgrParm.secret })
                            .with_issuer(tokenMgrParm.issuer)
                            .with_type(tokenMgrParm.type)
                            .with_subject(loggedinUserInfo.userName.value())
                            .with_id(std::to_string(loggedinUserInfo.userID.value()))
                            .with_claim("group", jwt::claim(loggedinUserInfo.group.value()))
                            .with_claim("llodt", jwt::claim(loggedinUserInfo.llodt.value_or("first_login")));

        // Verify the token
        verifier.verify(token);

        //  Check if token is still valid
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
