#pragma once

#include <chrono>
#include <crow.h>
#include <fmt/core.h>

struct ElapsedTime : crow::ILocalMiddleware {

    struct context {
        std::chrono::steady_clock::time_point start_time;
    };

    ElapsedTime() = default;
    ~ElapsedTime() = default;

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        (void)res;
        (void)req;
        ctx.start_time = std::chrono::steady_clock::now();
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        (void)req;
        if (res.code == crow::NOT_FOUND || res.code == crow::NO_CONTENT || res.code == crow::INTERNAL_SERVER_ERROR)
            return;
        // Calculate the duration of the request
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - ctx.start_time).count();
        // Log the request duration
        CROW_LOG_INFO << "ElapsedTime: Request to " << req.url << " took " << duration << "ms";
    }
};