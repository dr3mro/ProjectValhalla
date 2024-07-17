#pragma once

#include "crow.h"
#include "utils/dosdetector.hpp"
#include <memory>

struct RateLimit : crow::ILocalMiddleware {
public:
    struct context { };

    RateLimit(std::shared_ptr<DOSDetector> dos_detector)
        : dos_detector(dos_detector)
    {
    }

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        (void)ctx;
        if (dos_detector->id_dos_attack(std::ref(req), std::ref(res))) {
            res.code = 429;
            res.end("Too many requests.");
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        (void)req;
        (void)res;
        (void)ctx;
    }

private:
    std::shared_ptr<DOSDetector> dos_detector;
};
