#pragma once

#include "entities/base.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "jsoncons/basic_json.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include <any>
#include <regex>

using json = jsoncons::json;

class Entity : public Base {
public:
    using UserData = struct UserData {
        std::string username;
        std::string password;
        std::string password_hash;
        std::string email;
        std::string role;
        std::string user_data;

        bool validateUsername() const
        {
            const std::regex pattern("^[a-z][a-z0-9_]*$");
            return std::regex_match(username, pattern);
        }
        bool validatePassowrd() const
        {
            const std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");
            return std::regex_match(password, pattern);
        }
        bool validateEmail() const
        {
            const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
            return std::regex_match(email, pattern);
        }

        UserData(const json& _data, std::shared_ptr<PasswordCrypt>& passwordCrypt)
        {
            json response;
            try {
                // Extract the username from the JSON
                json payload = _data["payload"];

                username = payload.at("username").as<std::string>();
                password = payload.at("password").as<std::string>();
                password_hash = passwordCrypt->hashPassword(password).value();
                email = payload.at("basic_data").at("contact").at("email").as<std::string>();
                role = payload.at("basic_data").at("role").as<std::string>();
                user_data = payload.at("basic_data").as<std::string>();

                payload.erase("password");
            } catch (const std::exception& e) {
                std::cerr << fmt::format("failed to create user: {}\n", e.what());
            }
        }
    };

    using Credentials = struct Credentials {
        std::string username;
        std::string password;
    };

    using CreateData = struct CreateData {

        json payload;
        uint64_t next_id;
        CreateData(const json& _payload, const uint64_t nid)
        {
            payload = _payload;
            next_id = nid;
        }
        CreateData() = default;
    };

    using ReadData = struct ReadData {

        std::vector<std::string> schema;
        uint64_t id;
        ReadData(const std::vector<std::string>& _schema, const uint64_t _id)
        {
            schema = _schema;
            id = _id;
        }
        ReadData() = default;
    };

    using UpdateData = struct UpdateData {

        json payload;
        uint64_t user_id;
        UpdateData(const json& _data, const uint64_t id)
        {
            payload = _data;
            user_id = id;
        }
        UpdateData() = default;
    };

    using DeleteData = struct DeleteData {

        json payload;
        uint64_t user_id;
        DeleteData(const json& _data, const uint64_t id)
        {
            payload = _data;
            user_id = id;
        }
        DeleteData() = default;
    };

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

    using LogoutData = struct LogoutData {
        std::optional<std::string> token;
        LogoutData(const std::optional<std::string>& _token)
        {
            token = _token;
        }
    };

    template <typename T>
    Entity(const T& _data, const std::string& _tablename)
        : tablename(_tablename)
        , data(_data)
    {
    }

    ~Entity() override = default;

    std::optional<std::string> getSqlCreateStatement() override
    {
        std::optional<std::string> query;
        try {
            std::vector<std::string> keys_arr;
            std::vector<std::string> values_arr;

            json payload = std::any_cast<CreateData>(data).payload;
            uint64_t next_id = std::any_cast<CreateData>(data).next_id;
            for (auto& it : payload.object_range()) {

                if (it.key() == "basic_data" && it.value().contains("id")) {
                    it.value()["id"] = next_id;
                }

                keys_arr.push_back(it.key());
                values_arr.push_back(it.value().as<std::string>());
            }

            std::string columns = fmt::format("{}", fmt::join(keys_arr, ","));
            std::string values = fmt::format("'{}'", fmt::join(values_arr, "','"));

            query = fmt::format("INSERT INTO {} (id, {}) VALUES ({},{}) RETURNING id,{};", tablename, columns, next_id, values, columns);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for create " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    };
    std::optional<std::string> getSqlReadStatement() override
    {
        std::optional<std::string> query;
        try {
            auto user_id = std::any_cast<ReadData>(data).id;
            auto schema = std::any_cast<ReadData>(data).schema;

            std::string columns = fmt::format("{}", fmt::join(schema, ", "));

            query = fmt::format("SELECT {} FROM {} WHERE id={} LIMIT 1;", columns, tablename, user_id);
        } catch (const std::exception& e) {
            std::cerr << "faild to create query for read " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }
    std::optional<std::string> getSqlUpdateStatement() override
    {
        std::optional<std::string> query;

        try {
            json payload = std::any_cast<UpdateData>(data).payload;

            uint64_t id = std::any_cast<UpdateData>(data).user_id;
            ;

            std::string update_column_values;

            for (auto it = payload.object_range().begin(); it != payload.object_range().end(); ++it) {
                update_column_values.append(fmt::format(" {} = '{}' ", it->key(), it->value().as<std::string>()));
                if (std::next(it) != payload.object_range().end()) {
                    update_column_values.append(",");
                }
            }

            query = fmt::format("UPDATE {} set {} WHERE id={};", tablename, update_column_values, id);
        } catch (const std::exception& e) {
            std::cerr << "faild to create query for update " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }

    std::optional<std::string> getSqlDeleteStatement() override
    {
        std::optional<std::string> query;
        uint64_t id;

        try {
            jsoncons::json payload = std::any_cast<DeleteData>(data).payload;
            jsoncons::json basic_data = payload.at("basic_data");
            id = basic_data.at("id").as<uint64_t>();

            // Construct SQL query using {fmt} for parameterized query
            query
                = fmt::format("DELETE FROM {} where id={};", tablename, id);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for delete " << tablename << e.what() << std::endl;
            return std::nullopt;
        }
        return query;
    }
    std::optional<std::string> getSqlSearchStatement() override
    {
        std::optional<std::string> query;
        try {
            SearchData searchdata = std::any_cast<SearchData>(data);
            query
                = fmt::format("SELECT basic_data FROM {} WHERE basic_data::text ILIKE '%{}%' ORDER BY {} {} LIMIT {} OFFSET {};",
                    tablename,
                    searchdata.keyword,
                    searchdata.order_by,
                    searchdata.direction,
                    searchdata.limit + 1,
                    searchdata.offset);

        } catch (const std::exception& e) {
            std::cerr << "faild to create query for search " << tablename << e.what() << std::endl;
            return std::nullopt;
        }

        return query;
    }
    std::any getData() const
    {
        return data;
    }
    std::string getGroupName() const // ie. tablename
    {
        return tablename;
    }

protected:
    std::string tablename;

private:
    std::any data;
};
