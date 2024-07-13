#pragma once
#include "api_v1_routes.hpp"
#include "databaseconnectionpool.hpp"
#include "databasecontroller.hpp"
#include "memory"
#include "middlewares/authentication.hpp"
#include "middlewares/authorization.hpp"
#include "middlewares/dataintegrity.hpp"
#include "middlewares/elapsedtime.hpp"
#include "middlewares/search.hpp"
#include "middlewares/xrequest.hpp"
#include "patientcontroller.hpp"
#include "resthelper.hpp"
#include "sqlman.hpp"
#include "tokenizer.hpp"
#include "usercontroller.hpp"
#include <crow.h>
#include <crow/middlewares/cors.h>

class Server {
public:
    Server(uint16_t srv_threads, uint16_t db_connections);
    ~Server() = default;

    int run();

private:
    std::shared_ptr<DatabaseConnectionPool> dbConnPool;
    std::shared_ptr<DatabaseController> dbController;
    std::shared_ptr<RestHelper> restHelper;
    std::shared_ptr<Tokenizer> tokenizer;
    std::shared_ptr<SqlMan> sqlman;
    std::shared_ptr<PatientController> patientController;
    std::shared_ptr<UserController> userController;
    std::shared_ptr<ElapsedTime> elapsedTime;
    std::shared_ptr<Authentication> authentication;
    std::shared_ptr<Authorization> authorization;
    std::shared_ptr<DataIntegrity> dataIntegrity;
    std::shared_ptr<Search> search;
    std::shared_ptr<XRequest> xrequest;
    std::shared_ptr<crow::App<crow::CORSHandler, ElapsedTime, Authentication, Authorization, XRequest, Search, DataIntegrity>> app;
    std::shared_ptr<API_V1_Routes> routes;
    uint16_t srv_threads;
};