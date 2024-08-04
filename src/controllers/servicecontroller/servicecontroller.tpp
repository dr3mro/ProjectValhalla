
#pragma once
#include "controllers/controller/controller.hpp"
#include "controllers/servicecontroller/servicecontrollerbase.hpp"
#include "entities/entity.hpp"
#include "utils/resthelper/resthelper.hpp"
#include <crow.h>
#include <fmt/format.h>
#include <functional>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

template <typename T>
class ServiceController
    : public ServiceControllerBase
    , public Controller {
public:
    explicit ServiceController() = default;

    ~ServiceController() override = default;

    // CRUDS
    void CreateService(const crow::request &req, crow::response &res) override;
    void ReadService(const crow::request &req, crow::response &res, const jsoncons::json &criteria) override;
    void UpdateService(const crow::request &req, crow::response &res) override;
    void DeleteService(const crow::request &req, crow::response &res, const jsoncons::json &delete_json) override;
    void SearchService(const crow::request &req, crow::response &res, const jsoncons::json &search_json) override;

private:
    T service;
    uint64_t getNextID() override {
        json json_nextval = databaseController->executeQuery(fmt::format("SELECT NEXTVAL('{}_id_seq');", service.getTableName()));

        if (json_nextval.empty()) {
            return 0; // Or throw an exception if you prefer
        }

        for (const auto &obj : json_nextval.array_range()) {
            if (obj.contains("nextval")) {
                return obj["nextval"].as<uint64_t>();
            }
        }
        return 0;
    }
};


template <typename T>
void ServiceController<T>::CreateService(const crow::request &req, crow::response &res) {
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        Entity::CreateData createData(payload, getNextID());

        T service(createData);
        Controller::Create(std::ref(res), service);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ServiceController<T>::ReadService(const crow::request &req, crow::response &res, const json &criteria) {
    (void) req;
    json response;
    try {
        uint64_t id = criteria.at("id").as<uint64_t>();
        std::vector<std::string> schema = criteria.at("schema").as<std::vector<std::string>>();

        Entity::ReadData readData(schema, id);
        T service(readData);
        Controller::Read(std::ref(res), service);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ServiceController<T>::UpdateService(const crow::request &req, crow::response &res) {
    json response;
    try {
        json data(json::parse(req.body));
        json payload = data.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Entity::UpdateData updateData(payload, user_id);
        T service(updateData);
        Controller::Update(std::ref(res), service);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ServiceController<T>::DeleteService(const crow::request &req, crow::response &res, const jsoncons::json &delete_json) {
    (void) req;
    json response;
    try {
        json payload = delete_json.at("payload");
        json basic_data = payload.at("basic_data");
        uint64_t user_id = basic_data.at("id").as<uint64_t>();

        Entity::DeleteData deleteData(payload, user_id);
        T service(deleteData);
        Controller::Delete(std::ref(res), service);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

template <typename T>
void ServiceController<T>::SearchService(const crow::request &req, crow::response &res, const jsoncons::json &search_json) {
    (void) req;
    json response;
    try {
        Entity::SearchData searchData(search_json);
        T service(searchData);
        Controller::Search(std::ref(res), service);
    } catch (const std::exception &e) {
        rHelper->sendErrorResponse(std::ref(res), std::ref(response), "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
