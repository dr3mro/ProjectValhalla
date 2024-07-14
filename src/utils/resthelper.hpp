#pragma once
#include "controllers/databasecontroller.hpp"
#include <crow.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class RestHelper {
public:
    RestHelper(std::shared_ptr<DatabaseController> dbController);

    uint64_t get_next_patient_id();
    bool check_affected_rows(const json& response);
    void format_response(json& response_json, const short status, const std::string& status_message, const json& response);
    void finish_response(crow::response& res, const int& code, const json& response_json);
    void respond_with_error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code);
    bool get_nextid(uint64_t& nextid, crow::response& res);
    int evaluate_response(json& response_json, const json& query_results_json);
    void evaluate_and_finish(json& response_json, const json& query_results_json, crow::response& res);

private:
    std::shared_ptr<DatabaseController> dbController;
};
