#include "patientcontroller.hpp"
#include "entities/patient.hpp"
#include <functional>

PatientController::PatientController(const std::shared_ptr<DatabaseController>& dbController, const std::shared_ptr<RestHelper>& rHelper)
    : Controller(std::cref(dbController), std::cref(rHelper))
{
}

void PatientController::CreatePatient(const crow::request& req, crow::response& res)
{
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        Patient::CreatePatientData createpatientdata(payload, getNextID());
        Patient patient(createpatientdata);
        Controller::Create(std::ref(res), patient);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void PatientController::ReadPatient(crow::response& res, const json& criteria)
{
    json response;
    try {
        uint64_t id = criteria.at("id").as<uint64_t>();
        std::vector<std::string> schema = criteria.at("schema").as<std::vector<std::string>>();

        Patient::ReadPatientData readpatientdata(schema, id);
        Patient patient(readpatientdata);
        Controller::Read(std::ref(res), patient);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void PatientController::UpdatePatient(const crow::request& req, crow::response& res)
{
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Patient::UpdatePatientData updatepatientdata(payload, user_id);
        Patient patient(updatepatientdata);
        Controller::Update(std::ref(res), patient);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
void PatientController::DeletePatient(crow::response& res, const jsoncons::json& delete_json)
{
    json response;
    try {
        json payload = delete_json.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Patient::DeletePatientData deletepatientdata(payload, user_id);
        Patient patient(deletepatientdata);
        Controller::Delete(std::ref(res), patient);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void PatientController::SearchPatient(crow::response& res, const jsoncons::json& search_json)
{
    json response;
    try {
        Patient::SearchData searchpatientdata(search_json);
        Patient patient(searchpatientdata);
        Controller::Search(std::ref(res), patient);
    } catch (const std::exception& e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
