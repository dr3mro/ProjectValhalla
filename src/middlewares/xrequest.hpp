#pragma once

#include <crow.h>
#include <jsoncons/json.hpp>

struct XRequest : crow::ILocalMiddleware {

private:
    const std::string xRequestHeader = "X-Request";

public:
    struct context {
        jsoncons::json criteria;
    };

    XRequest() = default;
    ~XRequest() = default;

    void before_handle(crow::request &req, crow::response &res, context &ctx) {
        if (!(req.headers.contains(xRequestHeader) && !req.get_header_value(xRequestHeader).empty())) {
            res.code = 400;
            res.end("Missing proper request");
            return;
        }
        try {
            std::string decoded;
            std::optional<std::string> encoded = req.get_header_value(xRequestHeader);

            if (!encoded) {
                res.code = 400;
                res.end("no request provided");
                return;
            }

            jsoncons::decode_base64(encoded.value().begin(), encoded.value().end(), decoded);

            ctx.criteria = jsoncons::json::parse(decoded);

        } catch (const std::exception &e) {
            res.code = 500;
            res.end();
            return;
        }
    }

    void after_handle(crow::request &req, crow::response &res, context &ctx) {
        (void) req;
        (void) res;
        (void) ctx;
    }
};