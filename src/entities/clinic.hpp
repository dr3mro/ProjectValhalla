#pragma once

#include "entities/entity.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <jsoncons/json.hpp>
#include <optional>

using json = jsoncons::json;

class Clinic : public Entity {
public:
    using CreateClinicData = struct CreateClinicData {

        json payload;
        uint64_t next_id;
        CreateClinicData(const json& _payload, const uint64_t nid)
        {
            payload = _payload;
            next_id = nid;
        }
        CreateClinicData() = default;
    };

    using ReadClinicData = struct ReadClinicData {

        std::vector<std::string> schema;
        uint64_t id;
        ReadClinicData(const std::vector<std::string>& _schema, const uint64_t _id)
        {
            schema = _schema;
            id = _id;
        }
        ReadClinicData() = default;
    };

    using UpdateClinicData = struct UpdateClinicData {

        json payload;
        uint64_t user_id;
        UpdateClinicData(const json& _data, const uint64_t id)
        {
            payload = _data;
            user_id = id;
        }
        UpdateClinicData() = default;
    };

    using DeleteClinicData = struct DeleteClinicData {

        json payload;
        uint64_t user_id;
        DeleteClinicData(const json& _data, const uint64_t id)
        {
            payload = _data;
            user_id = id;
        }
        DeleteClinicData() = default;
    };

    template <typename T>
    Clinic(const T& _data)
    {
        data = _data;
    }

    ~Clinic() override = default;

    std::optional<std::string> getSqlCreateStatement() override
    {
        std::optional<std::string> query;
        try {
            std::vector<std::string> keys_arr;
            std::vector<std::string> values_arr;

            json payload = std::any_cast<CreateClinicData>(data).payload;
            uint64_t next_id = std::any_cast<CreateClinicData>(data).next_id;
            for (auto& it : payload.object_range()) {

                if (it.key() == "basic_data" && it.value().contains("id")) {
                    it.value()["id"] = next_id;
                }

                keys_arr.push_back(it.key());
                values_arr.push_back(it.value().as<std::string>());
            }

            std::string columns = fmt::format("{}", fmt::join(keys_arr, ","));
            std::string values = fmt::format("'{}'", fmt::join(values_arr, "','"));

            query = fmt::format("INSERT INTO clinics (id, {}) VALUES ({},{}) RETURNING id,{};", columns, next_id, values, columns);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for create_Clinic" << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    };
    std::optional<std::string> getSqlReadStatement() override
    {
        std::optional<std::string> query;
        try {
            auto user_id = std::any_cast<ReadClinicData>(data).id;
            auto schema = std::any_cast<ReadClinicData>(data).schema;

            std::string columns = fmt::format("{}", fmt::join(schema, ", "));

            query = fmt::format("SELECT {} FROM clinics WHERE id={} LIMIT 1;", columns, user_id);
        } catch (const std::exception& e) {
            std::cerr << "faild to create query for read_clinic" << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }
    std::optional<std::string> getSqlUpdateStatement() override
    {
        std::optional<std::string> query;

        try {
            json payload = std::any_cast<UpdateClinicData>(data).payload;

            uint64_t id = std::any_cast<UpdateClinicData>(data).user_id;
            ;

            std::string update_column_values;

            for (auto it = payload.object_range().begin(); it != payload.object_range().end(); ++it) {
                update_column_values.append(fmt::format(" {} = '{}' ", it->key(), it->value().as<std::string>()));
                if (std::next(it) != payload.object_range().end()) {
                    update_column_values.append(",");
                }
            }

            query = fmt::format("UPDATE clinics set {} WHERE id={};", update_column_values, id);
        } catch (const std::exception& e) {
            std::cerr << "faild to create query for update_clinic" << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }

    std::optional<std::string> getSqlDeleteStatement() override
    {
        std::optional<std::string> query;
        uint64_t id;

        try {
            jsoncons::json payload = std::any_cast<DeleteClinicData>(data).payload;
            jsoncons::json basic_data = payload.at("basic_data");
            id = basic_data.at("id").as<uint64_t>();

            // Construct SQL query using {fmt} for parameterized query
            query
                = fmt::format("DELETE FROM clinics where id={};", id);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for delete_clinic" << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }
    std::optional<std::string> getSqlSearchStatement() override
    {
        std::optional<std::string> query;
        try {
            SearchData searchClinicdata = std::any_cast<SearchData>(data);
            query
                = fmt::format("SELECT basic_data FROM clinics WHERE basic_data::text ILIKE '%{}%' ORDER BY {} {} LIMIT {} OFFSET {};",
                    searchClinicdata.keyword,
                    searchClinicdata.order_by,
                    searchClinicdata.direction,
                    searchClinicdata.limit + 1,
                    searchClinicdata.offset);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for search_Clinic" << e.what() << std::endl;
            return std::nullopt;
        }

        return query;
    }
};
