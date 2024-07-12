#pragma once

#include <crow.h>
#include <fmt/core.h>
#include <jsoncons/json.hpp>

struct Search : crow::ILocalMiddleware {

    struct context {
        jsoncons::json search_json;
    };
    Search() = default;
    ~Search() = default;

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        try {
            ctx.search_json = jsoncons::json::parse(req.body);

            std::vector<std::string> keys = {
                "keyword",
                "order_by",
                "direction",
                "limit",
                "offset"
            };

            auto it = keys.begin();

            while (it != keys.end()) {
                if (!ctx.search_json.contains(*it)) {
                    res.code = 400;
                    res.end(fmt::format("{} not found", *it));
                    return;
                }
                it++;
            }
            return;
        } catch (const std::exception& e) {

            res.code = 400;
            res.end("Inconsistent search data");
            return;
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        (void)req;
        (void)res;
        (void)ctx;
    }
};