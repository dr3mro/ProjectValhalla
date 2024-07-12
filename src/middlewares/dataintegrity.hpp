#pragma once

#include <crow.h>
#include <jsoncons/json.hpp>
#include <picosha2.h>

struct DataIntegrity : crow::ILocalMiddleware {

    struct context {
    };
    DataIntegrity() = default;
    ~DataIntegrity() = default;

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        (void)ctx;

        if (!(req.headers.contains("Enable-data-integrity-check") && req.get_header_value("Enable-data-integrity-check") == "true"))
            return;

        try {

            jsoncons::json json = jsoncons::json::parse(req.body);

            if (!json.contains("sha256sum") || !json.contains("payload")) {
                res.code = 400;
                res.end();
                return;
            }

            std::string sha256sum = json.at("sha256sum").as<std::string>();
            std::string payload = json.at("payload").as<std::string>();

            if (sha256sum != picosha2::hash256_hex_string(payload)) {
                res.code = 400;
                res.end();
                return;
            }
        } catch (const std::exception& e) {
            res.code = 400;
            res.end();
            return;
        }
    }

    void
    after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        (void)req;
        (void)res;
        (void)ctx;
    }
};