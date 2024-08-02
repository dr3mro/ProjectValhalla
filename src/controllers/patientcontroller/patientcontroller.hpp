#pragma once
#include "controllers/controller/controller.hpp"
#include "utils/resthelper/resthelper.hpp"
#include <crow.h>
#include <fmt/format.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class PatientController : public Controller {
public:
    PatientController() = default;
    ~PatientController() = default;

    // CRUDS
    void CreatePatient(const crow::request& req, crow::response& res);
    void ReadPatient(crow::response& res, const jsoncons::json& criteria);
    void UpdatePatient(const crow::request& req, crow::response& res);
    void DeletePatient(crow::response& res, const jsoncons::json& delete_json);
    void SearchPatient(crow::response& res, const jsoncons::json& search_json);

private:
    uint64_t getNextID()
    {
        json json_nextval = databaseController->executeQuery("SELECT NEXTVAL('patient_id_seq');");

        if (json_nextval.empty()) {
            return 0; // Or throw an exception if you prefer
        }

        for (const auto& obj : json_nextval.array_range()) {
            if (obj.contains("nextval")) {
                return obj["nextval"].as<uint64_t>();
            }
        }
        return 0;
    }
    const std::string tablename = "patients";
};
