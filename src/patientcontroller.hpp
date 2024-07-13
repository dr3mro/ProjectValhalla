#pragma once
#include "databasecontroller.hpp"
#include "resthelper.hpp"
#include "sqlman.hpp"
#include "tokenizer.hpp"
#include <crow.h>
#include <fmt/format.h>

using json = jsoncons::json;

class PatientController {
public:
    PatientController(std::shared_ptr<DatabaseController> dbController, std::shared_ptr<RestHelper> rHelper, std::shared_ptr<SqlMan> sqlman);

    // CRUDS
    void create_patient(const crow::request& req, crow::response& res);
    void read_patient(const crow::request& req, crow::response& res, const jsoncons::json& criteria);
    void update_patient(const crow::request& req, crow::response& res);
    void delete_patient(const crow::request& req, crow::response& res);
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
            rHelper->format_response(response_json, -1, "failure", "failed to synthesize query");
            rHelper->finish_response(res, 400, response_json);
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
            rHelper->evaluate_and_finish(response_json, query_results_json, res);
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            rHelper->format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
            rHelper->finish_response(res, 500, response_json);
        }
    }
};
