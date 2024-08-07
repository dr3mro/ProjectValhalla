#pragma once
#include "controllers/base/controller/controller.hpp"
#include "utils/resthelper/resthelper.hpp"
#include <crow.h>
#include <fmt/format.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

/**
 * @class PatientController
 * @brief Handles CRUD operations for patient data.
 *
 * The PatientController class provides methods for creating, reading, updating, deleting, and searching patient data. It inherits from the base Controller class and utilizes the RestHelper utility for interacting with the database.
 *
 * @method CreatePatient
 * @brief Creates a new patient record.
 * @param req The incoming HTTP request object.
 * @param res The outgoing HTTP response object.
 *
 * @method ReadPatient
 * @brief Retrieves a patient record based on the provided criteria.
 * @param res The outgoing HTTP response object.
 * @param criteria The JSON object containing the search criteria.
 *
 * @method UpdatePatient
 * @brief Updates an existing patient record.
 * @param req The incoming HTTP request object.
 * @param res The outgoing HTTP response object.
 *
 * @method DeletePatient
 * @brief Deletes a patient record based on the provided criteria.
 * @param res The outgoing HTTP response object.
 * @param delete_json The JSON object containing the delete criteria.
 *
 * @method SearchPatient
 * @brief Searches for patient records based on the provided criteria.
 * @param res The outgoing HTTP response object.
 * @param search_json The JSON object containing the search criteria.
 *
 * @method getNextID
 * @brief Retrieves the next available patient ID from the database.
 * @return The next available patient ID.
 */
class PatientController : public Controller {
public:
    PatientController() = default;
    ~PatientController() = default;

    // CRUDS
    void CreatePatient(const crow::request &req, crow::response &res);
    void ReadPatient(crow::response &res, const jsoncons::json &criteria);
    void UpdatePatient(const crow::request &req, crow::response &res);
    void DeletePatient(crow::response &res, const jsoncons::json &delete_json);
    void SearchPatient(crow::response &res, const jsoncons::json &search_json);

private:
    /**
     * @brief Retrieves the next available patient ID from the database.
     * @return The next available patient ID.
     */
    uint64_t getNextID() {
        json json_nextval = databaseController->executeQuery("SELECT NEXTVAL('patient_id_seq');");

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
    uint64_t getNextID() {
        json json_nextval = databaseController->executeQuery("SELECT NEXTVAL('patient_id_seq');");

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
    const std::string tablename = "patients";
};
