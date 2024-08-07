#pragma once
#include "controllers/base/clientcontroller/clientcontroller.tpp"
#include "entities/people/provider.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include <memory>

using json = jsoncons::json;
// The UserController class derives from ClientController<User>
/**
 * @class ProviderController
 * @brief Provides functionality for managing providers in the application.
 *
 * The ProviderController class derives from the ClientController<Provider> class and
 * provides methods for creating, authenticating, reading, updating, deleting, and
 * searching providers. It also provides a method for logging out users.
 *
 * The ProviderController class uses the SessionManager to manage user sessions.
 */
class ProviderController : public ClientController<Provider> {
public:
    ProviderController();

    // Destructor
    ~ProviderController() override = default;

    // Additional user-specific methods can be declared here
    void CreateProvider(const crow::request &req, crow::response &res);
    void AuthenticateProvider(crow::response &res, const json &credentials);
    void ReadProvider(crow::response &res, const json &criteria);
    void UpdateProvider(const crow::request &req, crow::response &res);
    void DeleteProvider(const crow::request &req, crow::response &res, const json &criteria);
    void SearchProvider(const crow::request &req, crow::response &res, const json &search_json);
    void LogoutUser(crow::response &res, const std::optional<std::string> &token);

private:
    std::shared_ptr<SessionManager> sessionManager;
};
