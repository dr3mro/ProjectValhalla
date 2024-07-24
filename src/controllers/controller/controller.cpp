#include "controllers/controller/controller.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <fmt/ranges.h>

Controller::Controller(const std::shared_ptr<DatabaseController>& dbController,
    const std::shared_ptr<RestHelper>& rHelper,
    const std::shared_ptr<Tokenizer>& tokenizer)
    : dbController(dbController)
    , rHelper(rHelper)
    , tokenizer(tokenizer)
{
}
// template <typename T>
// void Controller::Create(const crow::request& req, crow::response& res, T& entity)
// {
//     cruds(std::ref(res), entity, dbexec);
// }
template <typename T>
void Controller::Read(const crow::request& req, crow::response& res, const jsoncons::json& criteria, T& entity)
{
    cruds(std::ref(res), entity, dbrexec);
}
template <typename T>
void Controller::Update(const crow::request& req, crow::response& res, T& entity)
{
    cruds(std::ref(res), entity, dbexec);
}
template <typename T>
void Controller::Delete(const crow::request& req, crow::response& res, const jsoncons::json& delete_json, T& entity)
{
    cruds(std::ref(res), entity, dbexec);
}
template <typename T>
void Controller::Search(const crow::request& req, crow::response& res, const jsoncons::json& search_json, T& entity)
{
    // (void)req;
    // json response_json;
    // json query_results_json;
    // std::optional<std::string> query;

    // try {
    //     SqlMan::SearchData searchData(search_json);
    //     query = sqlman->get_search_patient_sql(searchData);
    //     if (query) {
    //         query_results_json = dbController->executeReadQuery(std::cref(query.value()));
    //         size_t results_count = query_results_json.size();

    //         if (results_count > searchData.limit) {
    //             response_json["more"] = true;
    //             response_json["offset"] = searchData.offset + searchData.limit;
    //             query_results_json.erase(query_results_json.array_range().end() - 1);

    //         } else {
    //             response_json["more"] = false;
    //             response_json["offset"] = 0;
    //         }
    //     }

    //     if (query_results_json.empty()) {
    //         rHelper->sendErrorResponse(res, std::ref(response_json), "failure: ", "not found", -1, 400);
    //     } else {
    //         rHelper->buildResponse(response_json, 0, "success", query_results_json);
    //         rHelper->sendResponse(res, 200, response_json);
    //     }
    // } catch (const std::exception& e) {
    //     // Handle exception (log, etc.)
    //     rHelper->sendErrorResponse(res, std::ref(response_json), "failure: ", fmt::format("failed: {}", e.what()), -2, 500);
    // }
}
