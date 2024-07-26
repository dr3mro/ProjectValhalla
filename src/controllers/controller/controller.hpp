#pragma once
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "entities/entity.hpp"
#include "utils/resthelper/resthelper.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <jsoncons/json.hpp>
using json = jsoncons::json;

class Controller {
public:
    Controller(const std::shared_ptr<DatabaseController>& dbController, const std::shared_ptr<RestHelper>& rHelper)
        : dbController(dbController)
        , rHelper(rHelper)
    {
    }

    // CRUDS
    template <typename T>
    void Create(crow::response& res, T& entity)
    {
        std::optional<std::string> (Entity::*sqlstatement)() = &Entity::getSqlCreateStatement;
        cruds(std::ref(res), entity, sqlstatement, dbexec);
    }

    template <typename T>
    void Read(crow::response& res, T& entity)
    {
        std::optional<std::string> (Entity::*sqlstatement)() = &Entity::getSqlReadStatement;
        cruds(std::ref(res), entity, sqlstatement, dbrexec);
    }
    template <typename T>
    void Update(crow::response& res, T& entity)
    {
        std::optional<std::string> (Entity::*sqlstatement)() = &Entity::getSqlUpdateStatement;
        cruds(std::ref(res), entity, sqlstatement, dbexec);
    }
    template <typename T>
    void Delete(crow::response& res, T& entity)
    {
        std::optional<std::string> (Entity::*sqlstatement)() = &Entity::getSqlDeleteStatement;
        cruds(std::ref(res), entity, sqlstatement, dbexec);
    }
    template <typename T>
    void Search(crow::response& res, T& entity)
    {
        json response_json;
        json query_results_json;
        std::optional<std::string> query;

        try {

            query = entity.getSqlSearchStatement();

            if (query) {
                query_results_json = dbController->executeReadQuery(std::cref(query.value()));
                size_t results_count = query_results_json.size();

                if (results_count > std::any_cast<Entity::SearchData>(entity.getData()).limit) {
                    response_json["more"] = true;
                    response_json["offset"] = std::any_cast<Entity::SearchData>(entity.getData()).offset + std::any_cast<Entity::SearchData>(entity.getData()).limit;
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
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            rHelper->sendErrorResponse(res, std::ref(response_json), "failure: ", fmt::format("failed: {}", e.what()), -2, 500);
        }
    }

protected:
    std::shared_ptr<DatabaseController> dbController;
    std::shared_ptr<RestHelper> rHelper;

    json (DatabaseController::*dbexec)(const std::string&) = &DatabaseController::executeQuery;
    json (DatabaseController::*dbrexec)(const std::string&) = &DatabaseController::executeReadQuery;
    ///////////////////////////
    template <typename T>
    bool get_sql_statement(json& response_json, crow::response& res, std::optional<std::string>& query, Entity& entity, T& sqlstatement)
    {
        query = (entity.*sqlstatement)();

        if (!query) {
            rHelper->buildResponse(std::ref(response_json), -1, "failure", "failed to synthesize query");
            rHelper->sendResponse(std::ref(res), 400, std::ref(response_json));
            return false;
        }
        return true;
    }

    template <typename T, typename S>
    void cruds(crow::response& res, T& entity, S sqlstatement, json (DatabaseController::*f)(const std::string&))
    {
        json response_json;
        json query_results_json;
        std::optional<std::string> query;
        try {
            if (get_sql_statement(response_json, res, query, entity, sqlstatement) && query.has_value()) {
                query_results_json = (*dbController.*f)(query.value());
            }
            rHelper->sendQueryResult(response_json, query_results_json, res);
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            rHelper->buildResponse(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
            rHelper->sendResponse(res, 500, response_json);
        }
    }
};
