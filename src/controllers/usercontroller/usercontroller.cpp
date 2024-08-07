#include "usercontroller.hpp"

#include "store/store.hpp"
#include <fmt/core.h>
#include <jsoncons/json.hpp>

/**
 * Constructs a UserController instance and initializes the SessionManager.
 * The SessionManager is retrieved from the Store and stored in the `sessionManager` member variable.
 * If the SessionManager cannot be retrieved, an exception is logged and the program exits with a failure status.
 */
UserController::UserController()

{
    try {
        sessionManager = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(Type::SessionManager));
    } catch (const std::exception &e) {
        std::cerr << "Exception in UserController constructor: " << e.what() << std::endl;
        EXIT_FAILURE;
    }
}

/**
 * Creates a new user based on the data provided in the HTTP request.
 *
 * @param req The HTTP request object containing the user data.
 * @param res The HTTP response object to be used for sending the response.
 */
void UserController::CreateUser(const crow::request &req, crow::response &res) {
    ClientController<User>::CreateUser(std::cref(req), std::ref(res));
}
/**
 * Authenticates a user based on the provided credentials and sets the login time for the user in the session manager.
 *
 * @param res The HTTP response object to be used for sending the response.
 * @param credentials The JSON object containing the user's credentials.
 * @return void
 */
void UserController::AuthenticateUser(crow::response &res, const jsoncons::json &credentials) {
    auto user_id = ClientController<User>::AuthenticateUser(std::ref(res), std::cref(credentials));
    if (user_id) {
        sessionManager->setNowLoginTime(user_id.value(), "users");
    }
}
/**
 * Reads a user based on the provided criteria and sends the response.
 *
 * @param res The HTTP response object to be used for sending the response.
 * @param criteria The JSON object containing the criteria to search for the user.
 */
void UserController::ReadUser(crow::response &res, const json &criteria) {
    ClientController<User>::ReadClient(std::ref(res), std::cref(criteria));
}

/**
 * Updates an existing user based on the data provided in the HTTP request.
 *
 * @param req The HTTP request object containing the updated user data.
 * @param res The HTTP response object to be used for sending the response.
 */
void UserController::UpdateUser(const crow::request &req, crow::response &res) {
    ClientController<User>::UpdateClient(std::cref(req), std::ref(res));
}

/**
 * Deletes a user based on the provided criteria.
 *
 * @param req The HTTP request object containing the criteria to delete the user.
 * @param res The HTTP response object to be used for sending the response.
 * @param criteria The JSON object containing the criteria to search for the user to delete.
 */
void UserController::DeleteUser(const crow::request &req, crow::response &res, const json &criteria) {
    ClientController<User>::DeleteClient(std::cref(req), std::ref(res), std::cref(criteria));
}

/**
 * Searches for users based on the provided search criteria and sends the response.
 *
 * @param req The HTTP request object containing the search criteria.
 * @param res The HTTP response object to be used for sending the response.
 * @param search_json The JSON object containing the search criteria.
 */
void UserController::SearchUser(const crow::request &req, crow::response &res, const json &search_json) {
    ClientController<User>::SearchClient(std::cref(req), std::ref(res), std::cref(search_json));
}
/**
 * Logs out a user based on the provided token.
 *
 * @param res The HTTP response object to be used for sending the response.
 * @param token The optional token to identify the user to be logged out.
 */
void UserController::LogoutUser(crow::response &res, const std::optional<std::string> &token) {
    ClientController<User>::LogoutClient(std::ref(res), std::cref(token));
}