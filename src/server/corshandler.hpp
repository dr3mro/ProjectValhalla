#pragma once

#include "crow/middlewares/cors.h"
#include <crow.h>

class CORSHandler {
public:
    CORSHandler(crow::CORSHandler& corsHandler)
    {
        corsHandler.global()
            .origin("*")
            .methods(crow::HTTPMethod::GET, crow::HTTPMethod::OPTIONS)
            .headers("Content-Type", "Accept-Encoding", "Origin", "Access-Control-Request-Method")
            .max_age(3600);

        corsHandler.prefix("/v1/patient")
            .origin("*")
            .methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST, crow::HTTPMethod::DELETE, crow::HTTPMethod::PATCH, crow::HTTPMethod::SEARCH, crow::HTTPMethod::OPTIONS)
            .headers("Content-Type", "Accept-Encoding", "Origin", "Access-Control-Request-Method", "Authorization")
            .max_age(7200)
            .allow_credentials();

        corsHandler.prefix("/v1/user")
            .origin("*")
            .methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
            .headers("Content-Type", "Accept-Encoding", "Origin", "Access-Control-Request-Method", "Authentication")
            .max_age(7200)
            .allow_credentials();
    }
    ~CORSHandler() = default;
};