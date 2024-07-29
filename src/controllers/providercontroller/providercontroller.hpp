#pragma once
#include "controllers/clientcontroller/clientcontroller.tpp"
#include "entities/provider.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include <memory>

using json = jsoncons::json;
// The UserController class derives from ClientController<User>
class ProviderController : public ClientController<Provider> {
public:
    ProviderController(const std::shared_ptr<DatabaseController>& dbController,
        const std::shared_ptr<RestHelper>& rHelper,
        const std::shared_ptr<TokenManager>& tokenManager,
        const std::shared_ptr<PasswordCrypt>& passwordCrypt);

    // Destructor
    ~ProviderController() override = default;

    // Additional user-specific methods can be declared here
    void CreateProvider(const crow::request& req, crow::response& res);
    void AuthenticateProvider(crow::response& res, const json& credentials);
    void ReadProvider(crow::response& res, const json& criteria);
    void UpdateProvider(const crow::request& req, crow::response& res);
    void DeleteProvider(const crow::request& req, crow::response& res, const json& criteria);
    void SearchProvider(const crow::request& req, crow::response& res, const json& search_json);
    void LogoutUser(crow::response& res, const std::optional<std::string>& token);

private:
    std::shared_ptr<SessionManager> providerSessionManager;
};
