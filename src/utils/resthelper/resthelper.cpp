#include "resthelper.hpp"
#include "fmt/core.h"

RestHelper::RestHelper(const std::shared_ptr<DatabaseController>& dbController)
    : dbController(dbController)
{
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
void RestHelper::buildResponse(json& response_json, const short status, const std::string& status_message, const json& results)
{
    response_json["status_id"] = status;
    response_json["status_message"] = status_message;
    response_json["payload"] = results;
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
