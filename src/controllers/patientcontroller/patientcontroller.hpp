#pragma once
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/sqlman/sqlman.hpp"
#include <crow.h>
#include <fmt/format.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class PatientController {
public:
    PatientController(std::shared_ptr<DatabaseController> dbController, std::shared_ptr<RestHelper> rHelper, std::shared_ptr<SqlMan> sqlman);

    // CRUDS
    void create_patient(const crow::request& req, crow::response& res);
    void read_patient(const crow::request& req, crow::response& res, const jsoncons::json& criteria);
    void update_patient(const crow::request& req, crow::response& res);
    void delete_patient(const crow::request& req, crow::response& res, const jsoncons::json& delete_json);
    void search_patient(const crow::request& req, crow::response& res, const jsoncons::json& search_json);

private:
    std::shared_ptr<DatabaseController> dbController;
    std::shared_ptr<RestHelper> rHelper;
    std::shared_ptr<SqlMan> sqlman;

    json (DatabaseController::*dbexec)(const std::string&) = &DatabaseController::executeQuery;
    json (DatabaseController::*dbrexec)(const std::string&) = &DatabaseController::executeReadQuery;
    ///////////////////////////
    template <typename T>
    bool get_sql_statement(json& response_json, crow::response& res, std::optional<std::string>& query, T w)
    {
        query = w();

        if (query->empty()) {
            rHelper->buildResponse(std::ref(response_json), -1, "failure", "failed to synthesize query");
            rHelper->sendResponse(std::ref(res), 400, std::ref(response_json));
            return false;
        }
        return true;
    }

    template <typename T>
    void crud_common(crow::response& res, T& w, json (DatabaseController::*f)(const std::string&))
    {
        json response_json;
        json query_results_json;
        std::optional<std::string> query;
        try {
            if (get_sql_statement(response_json, res, query, w) && query.has_value()) {
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
