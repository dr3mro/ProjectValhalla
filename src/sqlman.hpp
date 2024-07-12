#pragma once
#include <crow.h>
#include <jsoncons/json.hpp>
#include <optional>
#include <string>

class SqlMan {
public:
    typedef struct SearchData {
        std::string keyword;
        std::string order_by;
        std::string direction;
        size_t limit;
        size_t offset;

        SearchData(const jsoncons::json& search_json)
        {
            keyword = search_json.at("keyword").as<std::string>();
            order_by = search_json.at("order_by").as<std::string>();
            direction = search_json.at("direction").as<short>() == 0 ? "ASC" : "DESC";
            limit = search_json.at("limit").as<size_t>();
            offset = search_json.at("offset").as<size_t>();
        }
    } SearchData;

    std::optional<std::string> get_create_patient_sql(const crow::request& req, const crow::response& res, uint64_t nextid) const;
    std::optional<std::string> get_read_patient_sql(const crow::request& req, const crow::response& res);
    std::optional<std::string> get_update_patient_sql(const crow::request& req, const crow::response& res);
    std::optional<std::string> get_delete_patient_sql(const crow::request& req, const crow::response& res);
    std::optional<std::string> get_search_patient_sql(SqlMan::SearchData& searchData);

    SqlMan() = default;
    ~SqlMan() = default;

private:
    typedef struct PatientData {
        uint64_t user_id;
        std::vector<std::string> schema;
        std::string username;
        std::string token;

        void fill(const jsoncons::json& data)
        {
            user_id = data.at("id").as<uint64_t>();
            schema = data.at("schema").as<std::vector<std::string>>();
        }
    } PatientData;
};