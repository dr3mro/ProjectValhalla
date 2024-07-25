#pragma once

#include "jsoncons/basic_json.hpp"
#include <any>
#include <optional>
#include <string>

using json = jsoncons::json;

class Entity {
public:
    using SearchData = struct SearchData {
        std::string keyword;
        std::string order_by;
        std::string direction;
        size_t limit;
        size_t offset;

        SearchData(const json& search_json)
        {
            keyword = search_json.at("keyword").as<std::string>();
            order_by = search_json.at("order_by").as<std::string>();
            direction = search_json.at("direction").as<short>() == 0 ? "ASC" : "DESC";
            limit = search_json.at("limit").as<size_t>();
            offset = search_json.at("offset").as<size_t>();
        }
        SearchData() = default;
    };

    explicit Entity() = default;
    virtual ~Entity() = default;

    virtual std::optional<std::string> getSqlCreateStatement() = 0;
    virtual std::optional<std::string> getSqlReadStatement() = 0;
    virtual std::optional<std::string> getSqlUpdateStatement() = 0;
    virtual std::optional<std::string> getSqlDeleteStatement() = 0;
    virtual std::optional<std::string> getSqlSearchStatement() = 0;

    std::any getData()
    {
        return data;
    }

protected:
    std::any data;
};
