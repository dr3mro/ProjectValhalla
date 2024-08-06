/**
 * @brief The ClientController class is responsible for handling client-related operations, such as creating, authenticating, reading, updating, deleting, and
 * searching clients.
 *
 * The ClientController class is a template class that takes a type parameter `T`, which is expected to be a client entity class that inherits from the `Entity`
 * class. The class provides various member functions to perform CRUD (Create, Read, Update, Delete) operations on clients, as well as authentication and logout
 * functionality.
 *
 * The constructor of the ClientController class initializes the necessary dependencies, such as the TokenManager, PasswordCrypt, and SessionManager, by
 * retrieving them from the global Store.
 *
 * The public member functions of the ClientController class are:
 *
 * - `CreateUser`: Creates a new client user based on the provided data in the request body.
 * - `AuthenticateUser`: Authenticates a client user based on the provided credentials and returns the client ID if successful.
 * - `ReadClient`: Reads a client based on the provided criteria (e.g., ID and schema).
 * - `UpdateClient`: Updates a client based on the provided payload in the request body.
 * - `DeleteClient`: Deletes a client based on the provided payload in the request body.
 * - `SearchClient`: Searches for clients based on the provided search criteria.
 * - `LogoutClient`: Logs out a client based on the provided token.
 *
 * The protected member variables of the ClientController class are:
 *
 * - `tokenManager`: A shared pointer to the TokenManager instance.
 * - `passwordCrypt`: A shared pointer to the PasswordCrypt instance.
 * - `sessionManager`: A shared pointer to the SessionManager instance.
 */
// clientcontroller.hpp

#pragma once

#include "controllers/controller/controller.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/client.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"
#include <crow.h>
#include <fmt/core.h>
#include <jsoncons/json.hpp>
#include <memory>

using json = jsoncons::json;

template <typename T>
/**
 * @brief The ClientController class is responsible for handling client-related operations, such as creating, authenticating, reading, updating, deleting, and
 * searching clients.
 *
 * The ClientController class is a template class that takes a type parameter `T`, which is expected to be a client entity class that inherits from the `Entity`
 * class. The class provides various member functions to perform CRUD (Create, Read, Update, Delete) operations on clients, as well as authentication and logout
 * functionality.
 *
 * The constructor of the ClientController class initializes the necessary dependencies, such as the TokenManager, PasswordCrypt, and SessionManager, by
 * retrieving them from the global Store.
 */
class ClientController : public Controller {
public:
    ClientController() {
        try {
            tokenManager = std::any_cast<std::shared_ptr<TokenManager>>(Store::getObject(Type::TokenManager));
            passwordCrypt = std::any_cast<std::shared_ptr<PasswordCrypt>>(Store::getObject(Type::PasswordCrypt));
            sessionManager = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(Type::SessionManager));
        } catch (const std::exception &e) {
            std::cerr << "Exception in ClientController constructor: " << e.what() << std::endl;
            EXIT_FAILURE;
        }
    }

    ~ClientController() override = default;

    // PUBLIC
    void CreateUser(const crow::request &req, crow::response &res);
    std::optional<uint64_t> AuthenticateUser(crow::response &res, const jsoncons::json &credentials);
    void ReadClient(crow::response &res, const json &criteria);
    void UpdateClient(const crow::request &req, crow::response &res);
    void DeleteClient(const crow::request &req, crow::response &res, const json &delete_json);
    void SearchClient(const crow::request &req, crow::response &res, const json &search_json);
    void LogoutClient(crow::response &res, const std::optional<std::string> &token);

protected:
    std::shared_ptr<TokenManager> tokenManager;
    std::shared_ptr<PasswordCrypt> passwordCrypt;
    std::shared_ptr<SessionManager> sessionManager;
};

template <typename T>
/**
 * @brief Creates a new client user.
 *
 * This function is responsible for creating a new client user. It takes the request body as a JSON object, parses it into the appropriate user data type,
 * validates the user data, and then creates the new user.
 *
 * @param req The Crow request object containing the user data.
 * @param res The Crow response object to send the result of the operation.
 */
void ClientController<T>::CreateUser(const crow::request &req, crow::response &res) {
    json response;
    try {
        jsoncons::json data = jsoncons::json::parse(req.body);
        typename T::UserData user_data(data);

        T user(user_data);

        auto result = user.validate();

        if (!result.first) {
            rHelper->sendErrorResponse(res, response, "Failure", fmt::format("Failed: {}", result.second), -1, 400);
            return;
        }
        Create<T>(res, user);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(res, response, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
/**
 * @brief Authenticates a user and generates a token.
 *
 * This function is responsible for authenticating a user by verifying their credentials, and if successful, generating an authentication token that can be used
 * for subsequent requests.
 *
 * @param res The Crow response object to send the result of the operation.
 * @param credentials A JSON object containing the user's username and password.
 * @return std::optional<uint64_t> The user ID of the authenticated user, or std::nullopt if authentication failed.
 */
std::optional<uint64_t> ClientController<T>::AuthenticateUser(crow::response &res, const jsoncons::json &credentials) {
    json response;
    std::optional<uint64_t> client_id;
    try {
        typename T::Credentials creds;
        creds.username = credentials.at("username").as<std::string>();
        creds.password = credentials.at("password").as<std::string>();

        T client(creds);

        client_id = client.authenticate();

        if (!client_id) {
            rHelper->sendErrorResponse(res, response, "Login Failure", fmt::format("User '{}' not found or wrong password", creds.username), -1, 400);
            return std::nullopt;
        }

        TokenManager::LoggedUserInfo loggedUserInfo;

        loggedUserInfo.userID = client_id;
        loggedUserInfo.userName = creds.username;
        loggedUserInfo.group = client.getGroupName();
        loggedUserInfo.llodt = sessionManager->getLastLogoutTime(loggedUserInfo.userID.value(), loggedUserInfo.group.value()).value_or("first_login");

        json token_object;
        token_object["token"] = tokenManager->GenerateToken(loggedUserInfo);
        token_object["username"] = creds.username;
        token_object["user_id"] = client_id;
        token_object["group"] = loggedUserInfo.group;

        rHelper->buildResponse(response, 0, "success", token_object);
        rHelper->sendResponse(res, 200, response);
        return client_id;
    } catch (const std::exception &e) {
        // Handle exception (log, etc.)
        rHelper->sendErrorResponse(res, response, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
    return std::nullopt;
}

template <typename T>
/**
 * @brief Reads a client entity based on the provided criteria.
 *
 * This function is responsible for reading a client entity from the underlying data source based on the provided criteria. The criteria includes the client ID
 * and a list of schema fields to include in the response.
 *
 * @param res The Crow response object to send the result of the operation.
 * @param criteria A JSON object containing the client ID and the list of schema fields to include.
 */
void ClientController<T>::ReadClient(crow::response &res, const json &criteria) {
    json response;
    try {
        uint64_t id = criteria.at("id").as<uint64_t>();
        std::vector<std::string> schema = criteria.at("schema").as<std::vector<std::string>>();

        Entity::ReadData readData(schema, id);
        T client(readData);
        Controller::Read(std::ref(res), client);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
/**
 * @brief Updates a client entity based on the provided data.
 *
 * This function is responsible for updating a client entity in the underlying data source based on the provided data. The data includes the client ID and a
 * JSON payload containing the updated fields.
 *
 * @param req The Crow request object containing the update data.
 * @param res The Crow response object to send the result of the operation.
 */
void ClientController<T>::UpdateClient(const crow::request &req, crow::response &res) {
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Entity::UpdateData updateData(payload, user_id);
        T client(updateData);
        Controller::Update(std::ref(res), client);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
/**
 * @brief Deletes a client entity based on the provided data.
 *
 * This function is responsible for deleting a client entity from the underlying data source based on the provided data. The data includes the client ID.
 *
 * @param req The Crow request object containing the delete data.
 * @param res The Crow response object to send the result of the operation.
 * @param delete_json A JSON object containing the client ID and other data required for the delete operation.
 */
void ClientController<T>::DeleteClient(const crow::request &req, crow::response &res, const json &delete_json) {
    (void) req;
    json response;
    try {
        json payload = delete_json.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Entity::DeleteData deleteData(payload, user_id);
        T client(deleteData);
        Controller::Delete(std::ref(res), client);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
/**
 * @brief Searches for a client entity based on the provided data.
 *
 * This function is responsible for searching for a client entity in the underlying data source based on the provided search data. The search data is provided
 * in a JSON object.
 *
 * @param req The Crow request object containing the search data.
 * @param res The Crow response object to send the result of the search operation.
 * @param search_json A JSON object containing the search criteria.
 */
void ClientController<T>::SearchClient(const crow::request &req, crow::response &res, const json &search_json) {
    (void) req;
    json response;
    try {
        Entity::SearchData searchData(search_json);
        T client(searchData);
        Controller::Search(std::ref(res), client);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
/**
 * @brief Logs out a client by invalidating the provided token.
 *
 * This function is responsible for logging out a client by invalidating the provided token. It uses the `Entity::LogoutData` class to encapsulate the token
 * data and the `Controller::Logout` function to perform the logout operation.
 *
 * @param res The Crow response object to send the result of the logout operation.
 * @param token The optional token to be invalidated.
 */
void ClientController<T>::LogoutClient(crow::response &res, const std::optional<std::string> &token) {
    json response;
    try {
        Entity::LogoutData logoutData(token);
        T client(logoutData);
        Controller::Logout(std::ref(res), client);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}