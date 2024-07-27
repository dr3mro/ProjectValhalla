// clientcontroller.hpp

#pragma once

#include "controllers/controller/controller.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/client.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"
#include <crow.h>
#include <fmt/core.h>
#include <jsoncons/json.hpp>
#include <memory>

using json = jsoncons::json;

template <typename T>
class ClientController : public Controller {
public:
    explicit ClientController(const std::shared_ptr<DatabaseController>& dbController,
        const std::shared_ptr<RestHelper>& rHelper,
        const std::shared_ptr<TokenManager>& tokenManager,
        const std::shared_ptr<PasswordCrypt>& passwordCrypt);

    virtual ~ClientController() = default;

    // PUBLIC
    void CreateUser(const crow::request& req, crow::response& res);
    void AuthenticateUser(crow::response& res, const jsoncons::json& credentials);
    void ReadClient(crow::response& res, const json& criteria);
    void UpdateClient(const crow::request& req, crow::response& res);
    void DeleteClient(const crow::request& req, crow::response& res, const json& delete_json);
    void SearchClient(const crow::request& req, crow::response& res, const json& search_json);

protected:
    std::shared_ptr<TokenManager> tokenManager;
    std::shared_ptr<PasswordCrypt> passwordCrypt;
};

// Implementation
template <typename T>
ClientController<T>::ClientController(
    const std::shared_ptr<DatabaseController>& dbController,
    const std::shared_ptr<RestHelper>& rHelper,
    const std::shared_ptr<TokenManager>& tokenManager,
    const std::shared_ptr<PasswordCrypt>& passwordCrypt)
    : Controller(dbController, rHelper)
    , tokenManager(tokenManager)
    , passwordCrypt(passwordCrypt)
{
}

template <typename T>
void ClientController<T>::CreateUser(const crow::request& req, crow::response& res)
{
    json response;
    try {
        jsoncons::json data = jsoncons::json::parse(req.body);
        typename T::UserData user_data(data, passwordCrypt);

        T user(user_data, dbController, passwordCrypt);

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
void ClientController<T>::AuthenticateUser(crow::response& res, const jsoncons::json& credentials)
{
    json response;
    try {
        typename T::Credentials creds;
        creds.username = credentials["username"].as<std::string>();
        creds.password = credentials["password"].as<std::string>();

        T client(creds, dbController, passwordCrypt);

        uint64_t user_id = client.authenticate();

        if (user_id == 0) {
            rHelper->sendErrorResponse(res, response, "Login Failure", fmt::format("User '{}' not found or wrong password", creds.username), -1, 400);
            return;
        }

        TokenManager::LoggedUserInfo loggedUserInfo;
        loggedUserInfo.userID = user_id;
        loggedUserInfo.userName = creds.username;

        json token_object;
        token_object["token"] = tokenManager->GenerateToken(loggedUserInfo);
        token_object["username"] = creds.username;
        token_object["user_id"] = user_id;
        token_object["group"] = client.getGroupName();

        rHelper->buildResponse(response, 0, "success", token_object);
        rHelper->sendResponse(res, 200, response);
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper->sendErrorResponse(res, response, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ClientController<T>::ReadClient(crow::response& res, const json& criteria)
{
    json response;
    try {
        uint64_t id = criteria.at("id").as<uint64_t>();
        std::vector<std::string> schema = criteria.at("schema").as<std::vector<std::string>>();

        Entity::ReadData readData(schema, id);
        T client(readData, dbController, passwordCrypt);
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
        T client(updateData, dbController, passwordCrypt);
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
        T client(deleteData, dbController, passwordCrypt);
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
        T client(searchData, dbController, passwordCrypt);
        Controller::Search(std::ref(res), client);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}