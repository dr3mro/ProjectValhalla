#include "usercontroller.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include <fmt/core.h> // Include fmt library for string formatting
#include <jsoncons/json.hpp> // Include jsoncons library for JSON handling
#include <regex>
#include <utility>

// Definition of the implementation class
class UserController::Impl {
public:
    Impl(std::shared_ptr<DatabaseController> dbController, std::shared_ptr<RestHelper> rHelper, std::shared_ptr<Tokenizer> tokenizer)
        : dbController(std::move(dbController))
        , rHelper(std::move(rHelper))
        , tokenizer(std::move(tokenizer))
    {
    }

    using UserRegistrationData = struct {
        std::string username;
        std::string password_hash;
        std::string role;
        std::string user_data;
    };

    bool is_username_pattern_valid(const std::string& username);
    bool is_password_pattern_valid(const std::string& password);
    bool is_email_pattern_valid(const std::string& email);
    bool extract_and_sanity_check_user_registration_data(UserRegistrationData& userRegistrationData, json& userdata_json, json& response_json, crow::response& res);

    uint64_t authenticate_user(const std::string& username, const std::string& password) const;

    std::shared_ptr<DatabaseController> dbController;
    std::shared_ptr<RestHelper> rHelper;
    std::shared_ptr<Tokenizer> tokenizer;
    PasswordCrypt passwordCrypt;
};

bool UserController::Impl::is_username_pattern_valid(const std::string& username)
{
    const std::regex pattern("^[a-z][a-z0-9_]*$");
    return std::regex_match(username, pattern);
}

bool UserController::Impl::is_password_pattern_valid(const std::string& password)
{
    const std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");
    return std::regex_match(password, pattern);
}

bool UserController::Impl::is_email_pattern_valid(const std::string& email)
{
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    return std::regex_match(email, pattern);
}

bool UserController::Impl::extract_and_sanity_check_user_registration_data(UserRegistrationData& userRegistrationData, json& userdata_json, json& response_json, crow::response& res)
{
    try {
        // Extract the username from the JSON
        json payload = userdata_json["payload"];

        userRegistrationData.username = payload["username"].as<std::string>();
        std::string password = payload["password"].as<std::string>();

        // Check username pattern
        if (!is_username_pattern_valid(userRegistrationData.username)) {
            rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failed to create a new user: ", "Username contains invalid characters", -1, 400);
            return false;
        }
        if (!is_password_pattern_valid(password)) {
            rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failed to create a new user: ", "Password needs to be more secure", -1, 400);
            return false;
        }

        payload.erase("password");

        userRegistrationData.password_hash = passwordCrypt.hashPassword(password).value();
        userRegistrationData.role = payload["role"].as<std::string>();
        userRegistrationData.user_data = payload["user_data"].as<std::string>();

        std::string email = payload["user_data"]["contact"]["email"].as<std::string>();

        // Check if user exists
        if (dbController->checkItemExists("users", "username", userRegistrationData.username)) {
            rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failed to create a new user: ", "User already exists", -1, 400);
            return false;
        }

        // Check if username, password, or email are empty
        if (userRegistrationData.username.empty() || password.empty() || userRegistrationData.password_hash.empty()) {
            rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failed to create a new user, invalid data", "Empty username or password", -1, 400);
            return false;
        }

        // Check if the email matches the pattern
        if (!is_email_pattern_valid(std::ref(email))) {
            rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failed to create a new user, invalid data", "Invalid email format", -1, 400);
            return false;
        }
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
        return false;
    }
    return true;
}

uint64_t UserController::Impl::authenticate_user(const std::string& username, const std::string& password) const
{
    try {
        uint64_t user_id = dbController->findIfUserID(std::ref(username));

        if (user_id == 0)
            return 0;

        if (passwordCrypt.verifyPassword(password, dbController->getPasswordHashForUserID(user_id)))
            return user_id;
        else
            return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error authenticating user : " << e.what() << std::endl;
    }
    return 0;
}

UserController::UserController(std::shared_ptr<DatabaseController> dbController, std::shared_ptr<RestHelper> rHelper, std::shared_ptr<Tokenizer> tokenizer)
    : pImpl(std::make_unique<Impl>(std::move(dbController), std::move(rHelper), std::move(tokenizer)))
{
}

UserController::UserController()
    = default;

UserController::~UserController()
{
}

//////////////// PUBLIC /////////////////////////////

void UserController::register_user(const crow::request& req, crow::response& res)
{
    json response_json;
    json data_json;
    Impl::UserRegistrationData userRegistrationData;
    // Parse the JSON and extract the data
    try {
        // Get the JSON from request body
        data_json = jsoncons::json::parse(req.body);

        // Extract and validate user registration data
        if (!pImpl->extract_and_sanity_check_user_registration_data(std::ref(userRegistrationData), std::ref(data_json), std::ref(response_json), std::ref(res))) {
            return;
        }

        // Construct SQL query using {fmt} for parameterized query
        std::string query = fmt::format(
            "INSERT INTO users (username, password_hash, role, user_data) VALUES ('{}','{}','{}','{}')",
            userRegistrationData.username, userRegistrationData.password_hash, userRegistrationData.role, userRegistrationData.user_data);

        // Execute the query using DatabaseController
        json query_results_json = pImpl->dbController->executeQuery(std::ref(query));
        pImpl->rHelper->sendQueryResult(std::ref(response_json), std::ref(query_results_json), std::ref(res));
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        pImpl->rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void UserController::login_user(const crow::request& req, crow::response& res, const jsoncons::json& credentials)
{
    (void)req;
    json response_json;

    // Parse the JSON and extract the data
    try {

        std::string username = credentials.at("username").as<std::string>();
        std::string password = credentials.at("password").as<std::string>();

        uint64_t user_id = pImpl->authenticate_user(std::ref(username), std::ref(password));

        if (user_id == 0) {
            pImpl->rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Login Failure", fmt::format("User '{}' not found or wrong password", username), -1, 400);
            return;
        }

        json token_object;

        token_object["token"] = pImpl->tokenizer->generate_token(std::ref(username), std::to_string(user_id));
        token_object["username"] = username;
        token_object["user_id"] = user_id;

        pImpl->rHelper->buildResponse(std::ref(response_json), 0, "success", std::ref(token_object));
        pImpl->rHelper->sendResponse(std::ref(res), 200, std::ref(response_json));

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        pImpl->rHelper->sendErrorResponse(std::ref(res), std::ref(response_json), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}