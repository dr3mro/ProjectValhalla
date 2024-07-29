#include "providercontroller.hpp"
#include "store/store.hpp"
#include <fmt/core.h>
#include <jsoncons/json.hpp>

// Constructor for UserController, initializing the base class with User specialization
ProviderController::ProviderController(const std::shared_ptr<DatabaseController>& dbController,
    const std::shared_ptr<RestHelper>& rHelper,
    const std::shared_ptr<TokenManager>& tokenManager,
    const std::shared_ptr<PasswordCrypt>& passwordCrypt)
    : ClientController<Provider>(dbController, rHelper, tokenManager, passwordCrypt)

{
    providerSessionManager = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(Type::ProviderSessionManager));
}

// Here you can add any additional methods or override base class methods if needed

void ProviderController::CreateProvider(const crow::request& req, crow::response& res)
{
    ClientController<Provider>::CreateUser(req, res);
}
void ProviderController::AuthenticateProvider(crow::response& res, const json& credentials)
{
    auto provider_id = ClientController<Provider>::AuthenticateUser(std::ref(res), std::cref(credentials), std::ref(providerSessionManager));
    if (provider_id) {
        providerSessionManager->setNowLoginTime(provider_id.value());
    }
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
void ProviderController::LogoutUser(crow::response& res, const std::optional<std::string>& token)
{

    ClientController<Provider>::LogoutClient(std::ref(res), std::cref(token), providerSessionManager);
}