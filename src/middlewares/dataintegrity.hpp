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
                res.end("xxh64sum or payload not provided, aborting.");
                return;
            }

            std::string checksum = json.at("xxh64sum").as<std::string>();
            std::string payload = json.at("payload").as<std::string>();
            XXH64_hash_t hash = XXH64(payload.data(), payload.size(), 0);

            if (checksum != fmt::format("{:016x}", hash)) {
                res.code = 400;
                res.end("hash mismatch, aborting.");
                return;
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.end(fmt::format("Failure, {}", e.what()));
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