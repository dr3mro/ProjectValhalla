#include "providercontroller.hpp"
#include "store/store.hpp"
#include <fmt/core.h>
#include <jsoncons/json.hpp>

// Constructor for UserController, initializing the base class with User specialization
/**
 * Constructs a ProviderController instance and initializes the SessionManager object from the Store.
 * This constructor is responsible for setting up the necessary dependencies for the ProviderController class.
 * If the SessionManager object cannot be retrieved from the Store, an error message is printed and the program exits with a failure status.
 */
ProviderController::ProviderController() {
    try {
        sessionManager = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(Type::SessionManager));
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        EXIT_FAILURE;
    }
}

// Here you can add any additional methods or override base class methods if needed

/**
 * Creates a new Provider entity based on the data provided in the request.
 *
 * @param req The incoming HTTP request containing the Provider data.
 * @param res The HTTP response object to be used for sending the response.
 */
void ProviderController::CreateProvider(const crow::request &req, crow::response &res) {
    ClientController<Provider>::CreateUser(req, res);
}
/**
 * Authenticates a Provider and sets the login time for the Provider in the SessionManager.
 *
 * @param res The HTTP response object to be used for sending the response.
 * @param credentials The JSON object containing the Provider's credentials.
 * @return The Provider's ID if the authentication is successful, otherwise an empty optional.
 */
void ProviderController::AuthenticateProvider(crow::response &res, const json &credentials) {
    auto provider_id = ClientController<Provider>::AuthenticateUser(std::ref(res), std::cref(credentials));
    if (provider_id) {
        sessionManager->setNowLoginTime(provider_id.value(), "providers");
    }
}
/**
 * Reads a Provider entity based on the provided criteria.
 *
 * @param res The HTTP response object to be used for sending the response.
 * @param criteria The JSON object containing the search criteria for the Provider.
 */
void ProviderController::ReadProvider(crow::response &res, const json &criteria) {
    ClientController<Provider>::ReadClient(std::ref(res), std::cref(criteria));
}

/**
 * Updates an existing Provider entity based on the data provided in the request.
 *
 * @param req The incoming HTTP request containing the updated Provider data.
 * @param res The HTTP response object to be used for sending the response.
 */
void ProviderController::UpdateProvider(const crow::request &req, crow::response &res) {
    ClientController<Provider>::UpdateClient(std::cref(req), std::ref(res));
}

/**
 * Deletes a Provider entity based on the provided criteria.
 *
 * @param req The incoming HTTP request containing the search criteria for the Provider to be deleted.
 * @param res The HTTP response object to be used for sending the response.
 * @param criteria The JSON object containing the search criteria for the Provider to be deleted.
 */
void ProviderController::DeleteProvider(const crow::request &req, crow::response &res, const json &criteria) {
    ClientController<Provider>::DeleteClient(std::cref(req), std::ref(res), std::cref(criteria));
}

/**
 * Searches for Providers based on the provided search criteria.
 *
 * @param req The incoming HTTP request containing the search criteria.
 * @param res The HTTP response object to be used for sending the response.
 * @param search_json The JSON object containing the search criteria for the Providers.
 */
void ProviderController::SearchProvider(const crow::request &req, crow::response &res, const json &search_json) {
    ClientController<Provider>::SearchClient(std::cref(req), std::ref(res), std::cref(search_json));
}
/**
 * Logs out the Provider associated with the provided token.
 *
 * @param res The HTTP response object to be used for sending the response.
 * @param token The optional token representing the Provider to be logged out.
 */
void ProviderController::LogoutUser(crow::response &res, const std::optional<std::string> &token) {

    ClientController<Provider>::LogoutClient(std::ref(res), std::cref(token));
}