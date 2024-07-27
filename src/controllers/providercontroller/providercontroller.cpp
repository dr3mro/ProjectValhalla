#include "providercontroller.hpp"

#include <fmt/core.h>
#include <jsoncons/json.hpp>

// Constructor for UserController, initializing the base class with User specialization
ProviderController::ProviderController(const std::shared_ptr<DatabaseController>& dbController,
    const std::shared_ptr<RestHelper>& rHelper,
    const std::shared_ptr<TokenManager>& tokenManager,
    const std::shared_ptr<PasswordCrypt>& passwordCrypt)
    : ClientController<Provider>(dbController, rHelper, tokenManager, passwordCrypt)
{
}

// Here you can add any additional methods or override base class methods if needed

void ProviderController::CreateProvider(const crow::request& req, crow::response& res)
{
    ClientController<Provider>::CreateUser(req, res);
}
void ProviderController::AuthenticateProvider(crow::response& res, const json& credentials)
{
    ClientController<Provider>::AuthenticateUser(res, credentials);
}
void ProviderController::ReadProvider(crow::response& res, const json& criteria)
{
    ClientController<Provider>::ReadClient(std::ref(res), std::cref(criteria));
}

void ProviderController::UpdateProvider(const crow::request& req, crow::response& res)
{
    ClientController<Provider>::UpdateClient(std::cref(req), std::ref(res));
}

void ProviderController::DeleteProvider(const crow::request& req, crow::response& res, const json& criteria)
{
    ClientController<Provider>::DeleteClient(std::cref(req), std::ref(res), std::cref(criteria));
}

void ProviderController::SearchProvider(const crow::request& req, crow::response& res, const json& search_json)
{
    ClientController<Provider>::SearchClient(std::cref(req), std::ref(res), std::cref(search_json));
}