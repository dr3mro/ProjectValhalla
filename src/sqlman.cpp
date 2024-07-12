#include "sqlman.hpp"
#include <fmt/ranges.h>
#include <jsoncons/json.hpp>
#include <vector>

std::optional<std::string> SqlMan::get_create_patient_sql(const crow::request& req, const crow::response& res, uint64_t nextid) const
{
    (void)res;
    std::optional<std::string> query;
    try {
        std::vector<std::string> keys_arr;
        std::vector<std::string> values_arr;

        jsoncons::json json = jsoncons::json::parse(req.body);

        jsoncons::json payload = json.at("payload");

        for (auto it = payload.object_range().begin(); it != payload.object_range().end(); ++it) {

            if (it->key() == "basic_data" && it->value().contains("id")) {
                it->value()["id"] = nextid;
            }

            keys_arr.push_back(it->key());
            values_arr.push_back(it->value().as<std::string>());
        }

        std::string columns = fmt::format("{}", fmt::join(keys_arr, ","));
        std::string values = fmt::format("'{}'", fmt::join(values_arr, "','"));

        query = fmt::format("INSERT INTO patients (id, {}) VALUES ({},{}) RETURNING id,{};", columns, nextid, values, columns);

    } catch (const std::exception& e) {
        std::cerr << "faild to create query for create_patient" << e.what() << std::endl;
        return std::nullopt;
    }
    return query;
}
std::optional<std::string> SqlMan::get_read_patient_sql(const crow::request& req, const crow::response& res)
{
    (void)res;
    std::optional<std::string> query;
    PatientData patientData;
    try {
        jsoncons::json json = jsoncons::json::parse(req.body);
        patientData.fill(json);

        std::string columns = fmt::format("{}", fmt::join(patientData.schema, ", "));

        query = fmt::format("SELECT {} FROM patients WHERE id={} LIMIT 1;", columns, patientData.user_id);
    } catch (const std::exception& e) {
        std::cerr << "faild to create query for read_patient" << e.what() << std::endl;
        return std::nullopt;
    }
    return query;
}
std::optional<std::string> SqlMan::get_update_patient_sql(const crow::request& req, const crow::response& res)
{
    (void)res;
    std::optional<std::string> query;

    try {
        jsoncons::json json = jsoncons::json::parse(req.body);
        jsoncons::json payload = json.at("payload");

        uint64_t id = payload.at("basic_data").at("id").as<uint64_t>();

        std::string update_column_values;

        for (auto it = payload.object_range().begin(); it != payload.object_range().end(); ++it) {
            update_column_values.append(fmt::format(" {} = '{}' ", it->key(), it->value().as<std::string>()));
            if (std::next(it) != payload.object_range().end()) {
                update_column_values.append(",");
            }
        }

        query = fmt::format("UPDATE patients set {} WHERE id={};", update_column_values, id);

    } catch (const std::exception& e) {
        std::cerr << "faild to create query for update_patient" << e.what() << std::endl;
        return std::nullopt;
    }
    return query;
}
std::optional<std::string> SqlMan::get_delete_patient_sql(const crow::request& req, const crow::response& res)
{
    (void)res;
    std::optional<std::string> query;
    uint64_t id;
    std::string basic_data_str;

    try {
        jsoncons::json json = jsoncons::json::parse(req.body);
        jsoncons::json payload = json.at("payload");
        jsoncons::json basic_data = payload.at("basic_data");
        id = basic_data.at("id").as<uint64_t>();
        basic_data_str = basic_data.as<std::string>();

        // Construct SQL query using {fmt} for parameterized query
        query
            = fmt::format("DELETE FROM patients where id={} and basic_data @> '{}';", id, basic_data_str);

    } catch (const std::exception& e) {
        std::cerr << "faild to create query for delete_patient" << e.what() << std::endl;
        return std::nullopt;
    }
    return query;
}
std::optional<std::string> SqlMan::get_search_patient_sql(SqlMan::SearchData& searchData)
{
    std::optional<std::string> query;
    try {
        query
            = fmt::format("SELECT basic_data FROM patients WHERE basic_data::text ILIKE '%{}%' ORDER BY {} {} LIMIT {} OFFSET {};",
                searchData.keyword,
                searchData.order_by,
                searchData.direction,
                searchData.limit + 1,
                searchData.offset);

    } catch (const std::exception& e) {
        std::cerr << "faild to create query for search_patient" << e.what() << std::endl;
        return std::nullopt;
    }

    return query;
}