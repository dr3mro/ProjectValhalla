#pragma once

#include <crow.h>
#include <jsoncons/json.hpp>
#include <xxhash.h>

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

            if (!json.contains("xxh64sum") || !json.contains("payload")) {
                res.code = 400;
                res.end();
                return;
            }

            unsigned long long xxh64sum = json.at("xxh64sum").as<unsigned long long>();
            std::string payload = json.at("payload").as<std::string>();

            if (xxh64sum != XXH3_64bits(payload.c_str(), payload.size())) {
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