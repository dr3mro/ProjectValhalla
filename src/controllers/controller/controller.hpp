#pragma once
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/tokenizer/tokenizer.hpp"
#include <crow.h>
#include <fmt/format.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class Controller {
public:
    Controller(const std::shared_ptr<DatabaseController>& dbController, const std::shared_ptr<RestHelper>& rHelper, const std::shared_ptr<Tokenizer>& tokenizer);

    // CRUDS
    template <typename T>
    void Create(const crow::request& req, crow::response& res, T& entity)
    {
        cruds(std::ref(res), entity, dbexec);
    }

    template <typename T>
    void Read(const crow::request& req, crow::response& res, const jsoncons::json& criteria, T& entity);
    template <typename T>
    void Update(const crow::request& req, crow::response& res, T& entity);
    template <typename T>
    void Delete(const crow::request& req, crow::response& res, const jsoncons::json& delete_json, T& entity);
    template <typename T>
    void Search(const crow::request& req, crow::response& res, const jsoncons::json& search_json, T& entity);

protected:
    std::shared_ptr<DatabaseController> dbController;
    std::shared_ptr<RestHelper> rHelper;
    std::shared_ptr<Tokenizer> tokenizer;

    json (DatabaseController::*dbexec)(const std::string&) = &DatabaseController::executeQuery;
    json (DatabaseController::*dbrexec)(const std::string&) = &DatabaseController::executeReadQuery;
    ///////////////////////////
    template <typename T>
    bool get_sql_statement(json& response_json, crow::response& res, std::optional<std::string>& query, T entity)
    {
        query = entity.getSqlCreateStatement();

        if (query->empty()) {
            rHelper->buildResponse(std::ref(response_json), -1, "failure", "failed to synthesize query");
            rHelper->sendResponse(std::ref(res), 400, std::ref(response_json));
            return false;
        }
        return true;
    }

    template <typename T>
    void cruds(crow::response& res, T& entity, json (DatabaseController::*f)(const std::string&))
    {
        json response_json;
        json query_results_json;
        std::optional<std::string> query;
        try {
            if (get_sql_statement(response_json, res, query, entity) && query.has_value()) {
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
