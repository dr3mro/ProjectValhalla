#pragma once
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/entity.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <jsoncons/json.hpp>
using json = jsoncons::json;

class Controller {
public:
    Controller() {
        databaseController = std::any_cast<std::shared_ptr<DatabaseController>>(Store::getObject(Type::DatabaseController));
        rHelper = std::any_cast<std::shared_ptr<RestHelper>>(Store::getObject(Type::RestHelper));
        sessionManager = std::any_cast<std::shared_ptr<SessionManager>>(Store::getObject(Type::SessionManager));
        tokenManager = std::any_cast<std::shared_ptr<TokenManager>>(Store::getObject(Type::TokenManager));
    }
    virtual ~Controller() = default;

    // CRUDS
    template <typename T>
    void Create(crow::response &res, T &entity) {
        std::optional<std::string> (T::*sqlstatement)() = &T::getSqlCreateStatement;
        cruds(std::ref(res), entity, sqlstatement, dbexec);
    }

    template <typename T>
    void Read(crow::response &res, T &entity) {
        std::optional<std::string> (T::*sqlstatement)() = &T::getSqlReadStatement;
        cruds(std::ref(res), entity, sqlstatement, dbrexec);
    }
    template <typename T>
    void Update(crow::response &res, T &entity) {
        std::optional<std::string> (T::*sqlstatement)() = &T::getSqlUpdateStatement;
        cruds(std::ref(res), entity, sqlstatement, dbexec);
    }
    template <typename T>
    void Delete(crow::response &res, T &entity) {
        std::optional<std::string> (T::*sqlstatement)() = &T::getSqlDeleteStatement;
        cruds(std::ref(res), entity, sqlstatement, dbexec);
    }
    template <typename T>
    void Search(crow::response &res, T &entity) {
        json response_json;
        json query_results_json;
        std::optional<std::string> query;

        try {

            query = entity.getSqlSearchStatement();

            if (query) {
                query_results_json = databaseController->executeReadQuery(std::cref(query.value()));
                size_t results_count = query_results_json.size();

                if (results_count > std::any_cast<Entity::SearchData>(entity.getData()).limit) {
                    response_json["more"] = true;
                    response_json["offset"] = std::any_cast<Entity::SearchData>(entity.getData()).offset +
                        std::any_cast<Entity::SearchData>(entity.getData()).limit;
                    query_results_json.erase(query_results_json.array_range().end() - 1);

                } else {
                    response_json["more"] = false;
                    response_json["offset"] = 0;
                }
            }

            if (query_results_json.empty()) {
                rHelper->sendErrorResponse(res, std::ref(response_json), "failure: ", "not found", -1, 400);
            } else {
                rHelper->buildResponse(response_json, 0, "success", query_results_json);
                rHelper->sendResponse(res, 200, response_json);
            }
        } catch (const std::exception &e) {
            // Handle exception (log, etc.)
            rHelper->sendErrorResponse(res, std::ref(response_json), "failure: ", fmt::format("failed: {}", e.what()), -2, 500);
        }
    }

    template <typename T>
    void Logout(crow::response &res, T &entity) {

        TokenManager::LoggedUserInfo loggedUserInfo;
        loggedUserInfo.token = std::any_cast<Entity::LogoutData>(entity.getData()).token;
        loggedUserInfo.group = entity.getGroupName();

        bool status = tokenManager->ValidateToken(loggedUserInfo);
        if (!status) {
            res.code = 403;
            res.end("Failed to logout");
            return;
        }
        sessionManager->setNowLogoutTime(loggedUserInfo.userID.value(), loggedUserInfo.group.value());
        res.code = 200;
        res.end("logout success");
    }

protected:
    std::shared_ptr<DatabaseController> databaseController;
    std::shared_ptr<RestHelper> rHelper;
    std::shared_ptr<SessionManager> sessionManager;
    std::shared_ptr<TokenManager> tokenManager;

    std::optional<json> (DatabaseController::*dbexec)(const std::string &) = &DatabaseController::executeQuery;
    std::optional<json> (DatabaseController::*dbrexec)(const std::string &) = &DatabaseController::executeReadQuery;
    ///////////////////////////
    template <typename S, typename T>
    bool get_sql_statement(json &response_json, crow::response &res, std::optional<std::string> &query, T &entity, S &sqlstatement) {
        query = (entity.*sqlstatement)();

        if (!query) {
            rHelper->buildResponse(std::ref(response_json), -1, "failure", "failed to synthesize query");
            rHelper->sendResponse(std::ref(res), 400, std::ref(response_json));
            return false;
        }
        return true;
    }

    template <typename S, typename T>
    void cruds(crow::response &res, T &entity, S &sqlstatement, std::optional<json> (DatabaseController::*f)(const std::string &)) {
        json response_json;
        std::optional<json> query_results_json;
        std::optional<std::string> query;
        try {
            if (get_sql_statement(response_json, res, query, entity, sqlstatement) && query.has_value()) {
                query_results_json = (*databaseController.*f)(query.value());
            }
            rHelper->sendQueryResult(response_json, query_results_json.value(), res);
        } catch (const std::exception &e) {
            // Handle exception (log, etc.)
            rHelper->buildResponse(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
            rHelper->sendResponse(res, 500, response_json);
        }
    }
};
