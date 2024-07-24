#pragma once
#include "controllers/controller/controller.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/tokenizer/tokenizer.hpp"
#include <crow.h>
#include <fmt/format.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class PatientController : public Controller {
public:
    explicit PatientController(const std::shared_ptr<DatabaseController>& dbController, const std::shared_ptr<RestHelper>& rHelper, const std::shared_ptr<Tokenizer>& tokenizer);

    // CRUDS
    void create_patient(const crow::request& req, crow::response& res);
    void read_patient(const crow::request& req, crow::response& res, const jsoncons::json& criteria);
    void update_patient(const crow::request& req, crow::response& res);
    void delete_patient(const crow::request& req, crow::response& res, const jsoncons::json& delete_json);
    void search_patient(const crow::request& req, crow::response& res, const jsoncons::json& search_json);
};
