#include "usercontroller.hpp"

#include <fmt/core.h>
#include <jsoncons/json.hpp>

// Constructor for UserController, initializing the base class with User specialization
UserController::UserController(const std::shared_ptr<DatabaseController>& dbController,
    const std::shared_ptr<RestHelper>& rHelper,
    const std::shared_ptr<Tokenizer>& tokenizer,
    const std::shared_ptr<PasswordCrypt>& passwordCrypt)
    : ClientController<User>(dbController, rHelper, tokenizer, passwordCrypt)
{
}

// Here you can add any additional methods or override base class methods if needed

void UserController::CreateUser(const crow::request& req, crow::response& res)
{
    ClientController<User>::CreateUser(std::cref(req), std::ref(res));
}
void UserController::AuthenticateUser(crow::response& res, const jsoncons::json& credentials)
{
    ClientController<User>::AuthenticateUser(std::ref(res), std::cref(credentials));
}
void UserController::ReadUser(crow::response& res, const json& criteria)
{
    ClientController<User>::ReadClient(std::ref(res), std::cref(criteria));
}

void UserController::UpdateUser(const crow::request& req, crow::response& res)
{
    ClientController<User>::UpdateClient(std::cref(req), std::ref(res));
}

void UserController::DeleteUser(const crow::request& req, crow::response& res, const json& criteria)
{
    ClientController<User>::DeleteClient(std::cref(req), std::ref(res), std::cref(criteria));
}

void UserController::SearchUser(const crow::request& req, crow::response& res, const json& search_json)
{
    ClientController<User>::SearchClient(std::cref(req), std::ref(res), std::cref(search_json));
}