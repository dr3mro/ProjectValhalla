#pragma once
#include "controllers/databasecontroller/databasecontroller.hpp"
#include <crow.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class RestHelper {
public:
    RestHelper(std::shared_ptr<DatabaseController> dbController);

    uint64_t getNextPatientId();
    bool isQuerySuccessful(const json& response);
    void buildResponse(json& response_json, const short status, const std::string& status_message, const json& response);
    void sendResponse(crow::response& res, const int& code, const json& response_json);
    void sendErrorResponse(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code);
    bool getNextId(uint64_t& nextid, crow::response& res);
    int evaluateQueryResult(json& response_json, const json& query_results_json);
    void sendQueryResult(json& response_json, const json& query_results_json, crow::response& res);

private:
    std::shared_ptr<DatabaseController> dbController;
};
