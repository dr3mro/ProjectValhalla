#pragma once

#include "routes/api_v1_routes.hpp"

#include <crow.h>
#include <memory>

class Server {
public:
    Server(const uint16_t srv_threads, const uint16_t db_connections);
    ~Server() = default;

    int run();

private:
    uint16_t srv_threads;
    uint16_t db_connections;
    std::shared_ptr<APP> app;
    std::shared_ptr<API_V1_Routes> routes;

    void print_banner();
};