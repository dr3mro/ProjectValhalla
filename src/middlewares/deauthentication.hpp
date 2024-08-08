#pragma once
#include "jsoncons/basic_json.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"
#include <crow.h>
#include <optional>

using json = jsoncons::json;

struct Deauthentication : crow::ILocalMiddleware {

    struct context {
        std::optional<std::string> token;
    };
    Deauthentication() = default;
    ~Deauthentication() = default;

    void before_handle(crow::request &req, crow::response &res, context &ctx) {
        try {
            if (!req.headers.contains("Deauthentication")) {
                res.code = 403;
                res.end("Deauthentication Header not provided");
                return;
            }

            std::optional<std::string> raw_token = req.get_header_value("Deauthentication");

            if (!raw_token) {
                res.code = 403;
                res.end("Deauthentication data not provided");
                return;
            }

            if (raw_token.value().substr(0, 7) == "Bearer ") {
                ctx.token = raw_token.value().substr(7);
            }

            if (!ctx.token) {
                res.code = 403; // login denied
                res.end("Deauthentication token decoding failure");
            }

            return;
        } catch (const std::exception &e) {

            res.code = 500; // login denied
            res.end("Deauthentication Denied");
            return;
        }
    }
    void after_handle(crow::request &req, crow::response &res, context &ctx) {
        (void) req;
        (void) res;
        (void) ctx;
    }
};
