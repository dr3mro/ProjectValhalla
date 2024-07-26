#include "usercontroller.hpp"
#include "entities/user.hpp"
#include <fmt/core.h>
#include <functional>
#include <jsoncons/json.hpp>
#include <utility>

UserController::UserController(const std::shared_ptr<DatabaseController>& dbController, const std::shared_ptr<RestHelper>& rHelper, const std::shared_ptr<Tokenizer>& tokenizer)
    : Controller(dbController, rHelper)
    , tokenizer(tokenizer)

{
}

void UserController::CreateUser(const crow::request& req, crow::response& res)
{
    json response;
    try {
        json userdata = jsoncons::json::parse(req.body);
        User user(userdata, dbController);

        auto result = user.validate();

        if (!result.first) {
            rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", result.second), -1, 400);
            return;
        }
        Create<User>(res, user);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void UserController::AuthenticateUser(crow::response& res, const jsoncons::json& credentials)
{
    json response;

    try {

        std::string username = credentials.at("username").as<std::string>();
        std::string password = credentials.at("password").as<std::string>();
        User user(std::make_pair(username, password), dbController);

        uint64_t user_id = user.authenticate();

        if (user_id == 0) {
            rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Login Failure", fmt::format("User '{}' not found or wrong password", username), -1, 400);
            return;
        }

        json token_object;

        token_object["token"] = tokenizer->generate_token(std::ref(username), std::to_string(user_id));
        token_object["username"] = username;
        token_object["user_id"] = user_id;

        rHelper->buildResponse(std::ref(response), 0, "success", std::ref(token_object));
        rHelper->sendResponse(std::ref(res), 200, std::ref(response));

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}