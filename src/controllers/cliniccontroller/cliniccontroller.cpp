#include "cliniccontroller.hpp"
#include "entities/clinic.hpp"
#include <functional>

void ClinicController::CreateClinic(const crow::request& req, crow::response& res)
{
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        Clinic::CreateData createData(payload, getNextID());

        Clinic clinic(createData);
        Controller::Create(std::ref(res), clinic);
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

        Clinic::ReadData readData(schema, id);
        Clinic clinic(readData);
        Controller::Read(std::ref(res), clinic);
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

        Clinic::UpdateData updateData(payload, user_id);
        Clinic clinic(updateData);
        Controller::Update(std::ref(res), clinic);
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

        Clinic::DeleteData deleteData(payload, user_id);
        Clinic clinic(deleteData);
        Controller::Delete(std::ref(res), clinic);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void ClinicController::SearchClinic(crow::response& res, const jsoncons::json& search_json)
{
    json response;
    try {
        Clinic::SearchData searchData(search_json);
        Clinic clinic(searchData);
        Controller::Search(std::ref(res), clinic);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
