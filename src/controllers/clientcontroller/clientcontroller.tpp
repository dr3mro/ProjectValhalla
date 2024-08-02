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
class ClientController : public Controller {
public:
    ClientController()
    {
        tokenManager = std::any_cast<std::shared_ptr<TokenManager>>(Store::getObject(Type::TokenManager));
        passwordCrypt = std::any_cast<std::shared_ptr<PasswordCrypt>>(Store::getObject(Type::PasswordCrypt));
        sessionManager = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(Type::SessionManager));
    }

    virtual ~ClientController() = default;

    // PUBLIC
    void CreateUser(const crow::request& req, crow::response& res);
    std::optional<uint64_t> AuthenticateUser(crow::response& res, const jsoncons::json& credentials);
    void ReadClient(crow::response& res, const json& criteria);
    void UpdateClient(const crow::request& req, crow::response& res);
    void DeleteClient(const crow::request& req, crow::response& res, const json& delete_json);
    void SearchClient(const crow::request& req, crow::response& res, const json& search_json);
    void LogoutClient(crow::response& res, const std::optional<std::string>& token);

protected:
    std::shared_ptr<TokenManager> tokenManager;
    std::shared_ptr<PasswordCrypt> passwordCrypt;
    std::shared_ptr<SessionManager> sessionManager;
};

template <typename T>
void ClientController<T>::CreateUser(const crow::request& req, crow::response& res)
{
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
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(res, response, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
std::optional<uint64_t> ClientController<T>::AuthenticateUser(crow::response& res, const jsoncons::json& credentials)
{
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
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper->sendErrorResponse(res, response, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
    return std::nullopt;
}

template <typename T>
void ClientController<T>::ReadClient(crow::response& res, const json& criteria)
{
    json response;
    try {
        uint64_t id = criteria.at("id").as<uint64_t>();
        std::vector<std::string> schema = criteria.at("schema").as<std::vector<std::string>>();

        Entity::ReadData readData(schema, id);
        T client(readData);
        Controller::Read(std::ref(res), client);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ClientController<T>::UpdateClient(const crow::request& req, crow::response& res)
{
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Entity::UpdateData updateData(payload, user_id);
        T client(updateData);
        Controller::Update(std::ref(res), client);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ClientController<T>::DeleteClient(const crow::request& req, crow::response& res, const json& delete_json)
{
    (void)req;
    json response;
    try {
        json payload = delete_json.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Entity::DeleteData deleteData(payload, user_id);
        T client(deleteData);
        Controller::Delete(std::ref(res), client);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ClientController<T>::SearchClient(const crow::request& req, crow::response& res, const json& search_json)
{
    (void)req;
    json response;
    try {
        Entity::SearchData searchData(search_json);
        T client(searchData);
        Controller::Search(std::ref(res), client);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ClientController<T>::LogoutClient(crow::response& res, const std::optional<std::string>& token)
{
    json response;
    try {
        Entity::LogoutData logoutData(token);
        T client(logoutData);
        Controller::Logout(std::ref(res), client);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}