#pragma once
#include <crow.h>
#include <jsoncons/json.hpp>
#include <optional>
struct Authentication : crow::ILocalMiddleware {

    struct context {
        jsoncons::json credentials;
    };
    Authentication() = default;
    ~Authentication() = default;

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        try {
            std::string decoded;
            if (!req.headers.contains("Authentication")) {
                res.code = 403;
                res.end("Authentication Header not provided");
                return;
            }

            std::optional<std::string> encoded = req.get_header_value("Authentication");

            if (!encoded) {
                res.code = 403;
                res.end("Authentication data not provided");
                return;
            }

            jsoncons::decode_base64(encoded.value().begin(), encoded.value().end(), decoded);

            ctx.credentials = jsoncons::json::parse(decoded);

            return;

        } catch (const std::exception& e) {

            res.code = 500; // login denied
            res.end("Authentication Denied");
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
