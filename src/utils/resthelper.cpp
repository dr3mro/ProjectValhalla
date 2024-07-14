#include "resthelper.hpp"
#include "fmt/core.h"
#include <jwt-cpp/jwt.h>
#include <picosha2.h>

RestHelper::RestHelper(std::shared_ptr<DatabaseController> dbController)
    : dbController(dbController)
{
}

uint64_t RestHelper::get_next_patient_id()
{
    json json_nextval = dbController->executeQuery("SELECT NEXTVAL('patient_id_seq');");

    // Iterate through each object in the JSON array
    for (const auto& obj : json_nextval.array_range()) {
        if (obj.contains("nextval")) {
            return obj["nextval"].as<uint64_t>();
        }
    }
    return 0;
}

bool RestHelper::check_affected_rows(const json& response)
{
    if (response.empty())
        return false;

    for (const auto& obj : response.array_range()) {
        if (obj.contains("affected rows")) {
            return obj["affected rows"].as<uint64_t>() == 1;
        }
    }
    return true;
}

void RestHelper::format_response(json& response_json, const short status, const std::string& status_message, const json& response)
{
    response_json["status_id"] = status;
    response_json["status_message"] = status_message;
    response_json["payload"] = response;
}

void RestHelper::finish_response(crow::response& res, const int& code, const json& response_json)
{
    std::string body;
    response_json.dump(body, jsoncons::indenting::indent);
    res.code = code;
    res.write(body);
    res.end();
}

void RestHelper::respond_with_error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code)
{
    format_response(response_json, status, status_message, response);
    finish_response(res, code, response_json);
}

bool RestHelper::get_nextid(uint64_t& nextid, crow::response& res)
{
    json response_json;
    nextid = get_next_patient_id();

    if (nextid == 0) {
        format_response(response_json, -1, "failed to create a new patient", "failed to get nextval");
        finish_response(res, 400, response_json);
        return false;
    }
    return true;
}

int RestHelper::evaluate_response(json& response_json, const json& query_results_json)
{
    if (check_affected_rows(query_results_json)) {
        format_response(response_json, 0, "success", query_results_json);
        return 200;
    }
    format_response(response_json, -1, "failure", query_results_json);
    return 400;
}
void RestHelper::evaluate_and_finish(json& response_json, const json& query_results_json, crow::response& res)
{
    finish_response(res, evaluate_response(response_json, query_results_json), response_json);
}