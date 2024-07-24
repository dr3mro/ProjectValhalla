#include "resthelper.hpp"

#include "fmt/core.h"
#include <utility>

RestHelper::RestHelper(const std::shared_ptr<DatabaseController>& dbController)
    : dbController(dbController)
{
}

// Use a more descriptive name for the function
uint64_t RestHelper::getNextPatientId()
{
    json json_nextval = dbController->executeQuery("SELECT NEXTVAL('patient_id_seq');");

    // Handle the case where the query returns an empty result
    if (json_nextval.empty()) {
        return 0; // Or throw an exception if you prefer
    }

    // Iterate through each object in the JSON array
    for (const auto& obj : json_nextval.array_range()) {
        if (obj.contains("nextval")) {
            return obj["nextval"].as<uint64_t>();
        }
    }
    return 0; // Or throw an exception if you prefer
}

// Use a more descriptive name for the function
bool RestHelper::isQuerySuccessful(const json& response)
{
    if (response.empty()) {
        return false;
    }

    for (const auto& obj : response.array_range()) {
        if (obj.contains("affected rows")) {
            return obj["affected rows"].as<uint64_t>() == 1;
        }
    }
    return true;
}

// Use a more descriptive name for the function
void RestHelper::buildResponse(json& response_json, const short status, const std::string& status_message, const json& response)
{
    response_json["status_id"] = status;
    response_json["status_message"] = status_message;
    response_json["payload"] = response;
}

// Use a more descriptive name for the function
void RestHelper::sendResponse(crow::response& res, const int& code, const json& response_json)
{
    std::string body;
    response_json.dump(body, jsoncons::indenting::indent);
    res.code = code;
    res.write(body);
    res.end();
}

// Use a more descriptive name for the function
void RestHelper::sendErrorResponse(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code)
{
    buildResponse(response_json, status, status_message, response);
    sendResponse(res, code, response_json);
}

// Use a more descriptive name for the function
bool RestHelper::getNextId(uint64_t& nextid, crow::response& res)
{
    json response_json;
    nextid = getNextPatientId();

    if (nextid == 0) {
        buildResponse(response_json, -1, "failed to create a new patient", "failed to get nextval");
        sendResponse(res, 400, response_json);
        return false;
    }
    return true;
}

// Use a more descriptive name for the function
int RestHelper::evaluateQueryResult(json& response_json, const json& query_results_json)
{
    if (isQuerySuccessful(query_results_json)) {
        buildResponse(response_json, 0, "success", query_results_json);
        return 200;
    }
    buildResponse(response_json, -1, "failure", query_results_json);
    return 400;
}

// Use a more descriptive name for the function
void RestHelper::sendQueryResult(json& response_json, const json& query_results_json, crow::response& res)
{
    sendResponse(res, evaluateQueryResult(response_json, query_results_json), response_json);
}
