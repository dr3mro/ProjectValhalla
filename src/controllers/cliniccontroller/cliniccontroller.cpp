#include "cliniccontroller.hpp"
#include "entities/clinic.hpp"
#include <functional>

ClinicController::ClinicController(const std::shared_ptr<DatabaseController>& dbController, const std::shared_ptr<RestHelper>& rHelper)
    : Controller(std::cref(dbController), std::cref(rHelper))
{
}

void ClinicController::CreateClinic(const crow::request& req, crow::response& res)
{
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        Clinic::CreateClinicData createClinicdata(payload, getNextID());
        Clinic Clinic(createClinicdata);
        Controller::Create(std::ref(res), Clinic);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void ClinicController::ReadClinic(crow::response& res, const json& criteria)
{
    json response;
    try {
        uint64_t id = criteria.at("id").as<uint64_t>();
        std::vector<std::string> schema = criteria.at("schema").as<std::vector<std::string>>();

        Clinic::ReadClinicData readClinicdata(schema, id);
        Clinic Clinic(readClinicdata);
        Controller::Read(std::ref(res), Clinic);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void ClinicController::UpdateClinic(const crow::request& req, crow::response& res)
{
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Clinic::UpdateClinicData updateClinicdata(payload, user_id);
        Clinic Clinic(updateClinicdata);
        Controller::Update(std::ref(res), Clinic);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
void ClinicController::DeleteClinic(crow::response& res, const jsoncons::json& delete_json)
{
    json response;
    try {
        json payload = delete_json.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Clinic::DeleteClinicData deleteClinicdata(payload, user_id);
        Clinic Clinic(deleteClinicdata);
        Controller::Delete(std::ref(res), Clinic);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void ClinicController::SearchClinic(crow::response& res, const jsoncons::json& search_json)
{
    json response;
    try {
        Clinic::SearchData searchClinicdata(search_json);
        Clinic Clinic(searchClinicdata);
        Controller::Search(std::ref(res), Clinic);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
