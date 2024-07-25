#pragma once

#include "entities/entity.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <jsoncons/json.hpp>
#include <optional>

using json = jsoncons::json;

class Patient : public Entity {
public:
    using CreatePatientData = struct CreatePatientData {

        json payload;
        uint64_t next_id;
        CreatePatientData(const json& _payload, const uint64_t nid)
        {
            payload = _payload;
            next_id = nid;
        }
        CreatePatientData() = default;
    };

    using ReadPatientData = struct ReadPatientData {

        std::vector<std::string> schema;
        uint64_t id;
        ReadPatientData(const std::vector<std::string>& _schema, const uint64_t _id)
        {
            schema = _schema;
            id = _id;
        }
        ReadPatientData() = default;
    };

    using UpdatePatientData = struct UpdatePatientData {

        json payload;
        uint64_t user_id;
        UpdatePatientData(const json& _data, const uint64_t id)
        {
            payload = _data;
            user_id = id;
        }
        UpdatePatientData() = default;
    };

    using DeletePatientData = struct DeletePatientData {

        json payload;
        uint64_t user_id;
        DeletePatientData(const json& _data, const uint64_t id)
        {
            payload = _data;
            user_id = id;
        }
        DeletePatientData() = default;
    };

    template <typename T>
    Patient(const T& _data)
    {
        data = _data;
    }

    ~Patient() override = default;

    std::optional<std::string> getSqlCreateStatement() override
    {
        std::optional<std::string> query;
        try {
            std::vector<std::string> keys_arr;
            std::vector<std::string> values_arr;

            json payload = std::any_cast<CreatePatientData>(data).payload;
            uint64_t next_id = std::any_cast<CreatePatientData>(data).next_id;
            for (auto& it : payload.object_range()) {

                if (it.key() == "basic_data" && it.value().contains("id")) {
                    it.value()["id"] = next_id;
                }

                keys_arr.push_back(it.key());
                values_arr.push_back(it.value().as<std::string>());
            }

            std::string columns = fmt::format("{}", fmt::join(keys_arr, ","));
            std::string values = fmt::format("'{}'", fmt::join(values_arr, "','"));

            query = fmt::format("INSERT INTO patients (id, {}) VALUES ({},{}) RETURNING id,{};", columns, next_id, values, columns);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for create_patient" << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    };
    std::optional<std::string> getSqlReadStatement() override
    {
        std::optional<std::string> query;
        try {
            auto user_id = std::any_cast<ReadPatientData>(data).id;
            auto schema = std::any_cast<ReadPatientData>(data).schema;

            std::string columns = fmt::format("{}", fmt::join(schema, ", "));

            query = fmt::format("SELECT {} FROM patients WHERE id={} LIMIT 1;", columns, user_id);
        } catch (const std::exception& e) {
            std::cerr << "faild to create query for read_patient" << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }
    std::optional<std::string> getSqlUpdateStatement() override
    {
        std::optional<std::string> query;

        try {
            json payload = std::any_cast<UpdatePatientData>(data).payload;

            uint64_t id = std::any_cast<UpdatePatientData>(data).user_id;
            ;

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

    std::optional<std::string> getSqlDeleteStatement() override
    {
        std::optional<std::string> query;
        uint64_t id;
        std::string basic_data_str;

        try {
            jsoncons::json payload = std::any_cast<DeletePatientData>(data).payload;
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
    std::optional<std::string> getSqlSearchStatement() override
    {
        std::optional<std::string> query;
        try {
            SearchData searchpatientdata = std::any_cast<SearchData>(data);
            query
                = fmt::format("SELECT basic_data FROM patients WHERE basic_data::text ILIKE '%{}%' ORDER BY {} {} LIMIT {} OFFSET {};",
                    searchpatientdata.keyword,
                    searchpatientdata.order_by,
                    searchpatientdata.direction,
                    searchpatientdata.limit + 1,
                    searchpatientdata.offset);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for search_patient" << e.what() << std::endl;
            return std::nullopt;
        }

        return query;
    }
};
