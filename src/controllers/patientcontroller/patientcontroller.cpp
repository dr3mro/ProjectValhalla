#include "patientcontroller.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <fmt/ranges.h>
#include <functional>
#include <optional>
#include <utility>

PatientController::PatientController(std::shared_ptr<DatabaseController> dbController,
    std::shared_ptr<RestHelper> rHelper,
    std::shared_ptr<SqlMan> sqlman)
    : dbController(std::move(dbController))
    , rHelper(std::move(rHelper))
    , sqlman(std::move(sqlman))
{
}

void PatientController::create_patient(const crow::request& req, crow::response& res)
{

    auto w = [this, &req, &res]() {
        uint64_t nextid;
        if (!this->rHelper->getNextId(nextid, res)) {
            return std::optional<std::string>();
        }
        return this->sqlman->get_create_patient_sql(std::cref(req), std::cref(res), nextid);
    };

    crud_common(std::ref(res), w, dbexec);
}

void PatientController::read_patient(const crow::request& req, crow::response& res, const jsoncons::json& criteria)
{
    auto w = [this, &req, &res, &criteria]() { return this->sqlman->get_read_patient_sql(std::cref(req), std::cref(res), std::cref(criteria)); };
    crud_common(std::ref(res), w, dbrexec);
}

void PatientController::update_patient(const crow::request& req, crow::response& res)
{
    auto w = [this, &req, &res]() { return this->sqlman->get_update_patient_sql(std::cref(req), std::cref(res)); };
    crud_common(std::ref(res), w, dbexec);
}
void PatientController::delete_patient(const crow::request& req, crow::response& res, const jsoncons::json& delete_json)
{
    auto w = [this, &req, &res, &delete_json]() { return this->sqlman->get_delete_patient_sql(std::cref(req), std::cref(res), std::cref(delete_json)); };
    crud_common(std::ref(res), w, dbexec);
}

void PatientController::search_patient(const crow::request& req, crow::response& res, const jsoncons::json& search_json)
{
    (void)req;
    json response_json;
    json query_results_json;
    std::optional<std::string> query;

    try {
        SqlMan::SearchData searchData(search_json);
        query = sqlman->get_search_patient_sql(searchData);
        if (query) {
            query_results_json = dbController->executeReadQuery(std::cref(query.value()));
            size_t results_count = query_results_json.size();

            if (results_count > searchData.limit) {
                response_json["more"] = true;
                response_json["offset"] = searchData.offset + searchData.limit;
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
