#pragma once
#include "store/store.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"
#include <crow.h>
#include <jsoncons/json.hpp>
struct Authorization : crow::ILocalMiddleware {

    struct context {
        TokenManager::LoggedUserInfo userInfo;
    };
    Authorization() {
        tokenManager = std::any_cast<std::shared_ptr<TokenManager>>(Store::getObject(Type::TokenManager));
    }

    ~Authorization() = default;

    void before_handle(crow::request &req, crow::response &res, context &ctx) {
        try {
            std::optional<std::string> authorization = req.get_header_value("Authorization");

            if (!authorization) {
                res.code = 403;
                res.end("Authorization Header not found");
                return;
            }

            if (authorization.value().substr(0, 7) == "Bearer ") {
                ctx.userInfo.token = authorization.value().substr(7);
            }

            if (!ctx.userInfo.token) {
                // Respond with a 400 Bad Request if shasum parsing failed
                res.code = 403;
                res.end("Authorization Token not found");
                return;
            }

            if (!tokenManager->ValidateToken(ctx.userInfo)) {
                res.code = 403;
                res.end("Authorization denied");
                return;
            }
            return;
        } catch (const std::exception &e) {
            res.code = 500; // login denied
            res.end();
            return;
        }

        res.code = 403; // login denied
        res.end();
    }
    void after_handle(crow::request &req, crow::response &res, context &ctx) {
        (void) req;
        (void) res;
        (void) ctx;
    }

private:
    std::shared_ptr<TokenManager> tokenManager;
};
