#include "server.hpp"
#include <fmt/core.h>

#define PORT 8080

Server::Server(uint16_t srv_threads, uint16_t db_connections)
    : dbConnPool(std::make_shared<DatabaseConnectionPool>(db_connections))
    , dbController(std::make_shared<DatabaseController>(dbConnPool))
    , restHelper(std::make_shared<RestHelper>(dbController))
    , tokenizer(std::make_shared<Tokenizer>())
    , sqlman(std::make_shared<SqlMan>())
    , patientController(std::make_shared<PatientController>(dbController, restHelper, sqlman))
    , userController(std::make_shared<UserController>(dbController, restHelper, tokenizer, sqlman))
    , elapsedTime(std::make_shared<ElapsedTime>())
    , authentication(std::make_shared<Authentication>())
    , authorization(std::make_shared<Authorization>(tokenizer))
    , dataIntegrity(std::make_shared<DataIntegrity>())
    , search(std::make_shared<Search>())
    , xrequest(std::make_shared<XRequest>())
    , app(std::make_shared<crow::App<crow::CORSHandler, ElapsedTime, Authentication, Authorization, XRequest, Search, DataIntegrity>>(*elapsedTime, *authentication, *authorization, *xrequest, *search, *dataIntegrity))
    , routes(std::make_shared<API_V1_Routes>(app, userController, patientController))
    , srv_threads(srv_threads)
{
    auto& cors = app->get_middleware<crow::CORSHandler>();

    cors.global()
        .origin("*")
        .methods(crow::HTTPMethod::GET, crow::HTTPMethod::OPTIONS)
        .headers("Content-Type", "Accept-Encoding", "Origin", "Access-Control-Request-Method")
        .max_age(3600);

    cors.prefix("/v1/patient")
        .origin("*")
        .methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST, crow::HTTPMethod::DELETE, crow::HTTPMethod::PATCH, crow::HTTPMethod::SEARCH, crow::HTTPMethod::OPTIONS)
        .headers("Content-Type", "Accept-Encoding", "Origin", "Access-Control-Request-Method", "Authorization")
        .max_age(7200)
        .allow_credentials();

    cors.prefix("/v1/user")
        .origin("*")
        .methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
        .headers("Content-Type", "Accept-Encoding", "Origin", "Access-Control-Request-Method", "Authentication")
        .max_age(7200)
        .allow_credentials();
}

int Server::run()
{
    try {
        // Start the server on port %PORT%
        std::cout << fmt::format("database server is started on port {}.\n", PORT);

        app->loglevel(crow::LogLevel::INFO)
            .use_compression(crow::compression::algorithm::GZIP)
            .port(PORT)
            .multithreaded()
            .concurrency(srv_threads)
            .bindaddr("0.0.0.0")
            .server_name("ProjectValhalla")
            .run();

    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    return 0;
}