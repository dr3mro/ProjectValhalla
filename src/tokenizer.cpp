#include "tokenizer.hpp"

#define ISSUER "ProjectValhalla"
#define TYPE "JWS"
#define VALIDITY 43200
#define SECRET "01234567890123456789012345678901"
// Function to generate JWT token
std::string Tokenizer::generate_token(const std::string& username, const std::string& user_id)
{
    try {
        // Create JWT token with payload
        auto token = jwt::create()
                         .set_issuer(ISSUER)
                         .set_type(TYPE)
                         .set_subject(username)
                         .set_id(user_id)
                         .set_issued_at(std::chrono::system_clock::now())
                         .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes { VALIDITY })
                         .sign(jwt::algorithm::hs256 { SECRET });

        return token;
    } catch (const std::exception& e) {
        std::cerr << "Error generating token: " << e.what() << std::endl;
        return "";
    }
}

bool Tokenizer::token_validator(LoggedUserInfo& userinfo)
{
    try {
        auto verifier = jwt::verify()
                            .with_issuer(ISSUER)
                            .with_type(TYPE)
                            .allow_algorithm(jwt::algorithm::hs256 { SECRET });

        auto decoded_token = jwt::decode(userinfo.token.value());

        verifier.verify(decoded_token);

        // Check expiration

        auto exp_time = decoded_token.get_payload_claim("exp").as_date().time_since_epoch();
        auto issuer = decoded_token.get_payload_claim("iss").as_string();
        auto now = std::chrono::system_clock::now().time_since_epoch();

        userinfo.user_id = std::stoull(decoded_token.get_id());
        userinfo.username = decoded_token.get_subject();

        if (now < exp_time && issuer == ISSUER)
            return true;

    } catch (const std::exception& e) {
        std::cerr << "Error Validating token: " << e.what() << std::endl;
    }
    return false;
}