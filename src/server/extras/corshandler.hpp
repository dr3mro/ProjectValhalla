#pragma once

#include <crow.h>
#include <crow/middlewares/cors.h>

class CORSHandler {
public:
    template <typename App>
    CORSHandler(App app)
    {
        auto& cors = app->template get_middleware<crow::CORSHandler>();

        // Global CORS settings
        cors.global()
            .origin("*")
            .methods("GET"_method)
            .headers("Content-Type", "Accept-Encoding", "Origin")
            .max_age(3600)
            .allow_credentials();

        // CORS settings for /v1/patient prefix
        cors.prefix("/v1/patient")
            .origin("*")
            .methods("GET"_method, "POST"_method, "DELETE"_method, "PATCH"_method, "SEARCH"_method)
            .headers("Content-Type", "Accept-Encoding", "Origin", "Authorization")
            .max_age(7200)
            .allow_credentials();

        // CORS settings for /v1/user prefix
        cors.prefix("/v1/user")
            .origin("*")
            .methods("GET"_method, "POST"_method)
            .headers("Content-Type", "Accept-Encoding", "Origin", "Authentication")
            .max_age(7200)
            .allow_credentials();

        // CORS settings for /v1/hello prefix
        cors.prefix("/v1/hello")
            .origin("*")
            .methods("GET"_method)
            .headers("Content-Type", "Accept-Encoding", "Origin")
            .max_age(7200)
            .allow_credentials();
    }

    ~CORSHandler() = default;
};
