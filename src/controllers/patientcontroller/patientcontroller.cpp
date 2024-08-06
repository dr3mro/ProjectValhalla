#include "patientcontroller.hpp"
#include "entities/patient.hpp"
#include "utils/resthelper/resthelper.hpp"
#include <functional>

void PatientController::CreatePatient(const crow::request &req, crow::response &res) {
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        Patient::CreateData createData(payload, getNextID());
        Patient patient(createData);
        Controller::Create(std::ref(res), patient);
    } catch (const std::exception &e) {
        RestHelper::sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void PatientController::ReadPatient(crow::response &res, const json &criteria) {
    json response;
    try {
        uint64_t id = criteria.at("id").as<uint64_t>();
        std::vector<std::string> schema = criteria.at("schema").as<std::vector<std::string>>();

        Patient::ReadData readData(schema, id);
        Patient patient(readData);
        Controller::Read(std::ref(res), patient);
    } catch (const std::exception &e) {
        RestHelper::sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void PatientController::UpdatePatient(const crow::request &req, crow::response &res) {
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Patient::UpdateData updateData(payload, user_id);
        Patient patient(updateData);
        Controller::Update(std::ref(res), patient);
    } catch (const std::exception &e) {
        RestHelper::sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
void PatientController::DeletePatient(crow::response &res, const jsoncons::json &delete_json) {
    json response;
    try {
        json payload = delete_json.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Patient::DeleteData deleteData(payload, user_id);
        Patient patient(deleteData);
        Controller::Delete(std::ref(res), patient);
    } catch (const std::exception &e) {
        RestHelper::sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void PatientController::SearchPatient(crow::response &res, const jsoncons::json &search_json) {
    json response;
    try {
        Patient::SearchData searchData(search_json);
        Patient patient(searchData);
        Controller::Search(std::ref(res), patient);
    } catch (const std::exception &e) {
        RestHelper::sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
