#include "server.hpp"
#include "banner.hpp"
#include "corshandler.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

#define PORT 8080
#ifndef GIT_TAG
#define GIT_TAG "unknown"
#endif

Server::Server(uint16_t srv_threads, uint16_t db_connections)
    : dbConnPool(std::make_shared<DatabaseConnectionPool>(db_connections))
    , dbController(std::make_shared<DatabaseController>(dbConnPool))
    , restHelper(std::make_shared<RestHelper>(dbController))
    , tokenizer(std::make_shared<Tokenizer>())
    , sqlman(std::make_shared<SqlMan>())
    , patientController(std::make_shared<PatientController>(dbController, restHelper, sqlman))
    , userController(std::make_shared<UserController>(dbController, restHelper, tokenizer))
    , dos_detector(std::make_shared<DOSDetector>())
    , rateLimit(std::make_shared<RateLimit>(dos_detector))
    , elapsedTime(std::make_shared<ElapsedTime>())
    , authentication(std::make_shared<Authentication>())
    , authorization(std::make_shared<Authorization>(tokenizer))
    , dataIntegrity(std::make_shared<DataIntegrity>())
    , search(std::make_shared<Search>())
    , xrequest(std::make_shared<XRequest>())
    , app(std::make_shared<crow::App<crow::CORSHandler, RateLimit, ElapsedTime, Authentication, Authorization, XRequest, Search, DataIntegrity>>(*rateLimit, *elapsedTime, *authentication, *authorization, *xrequest, *search, *dataIntegrity))
    , routes(std::make_shared<API_V1_Routes>(app, userController, patientController))
    , srv_threads(srv_threads)
    , db_connections(db_connections)
{
}

int Server::run()
{
    print_banner();

    crow::CORSHandler& crowCorsHandler = app->get_middleware<crow::CORSHandler>();
    CORSHandler corsHandler(std::ref(crowCorsHandler)); // Apply CORS settings

    try {
        app->loglevel(crow::LogLevel::INFO)
            .use_compression(crow::compression::algorithm::GZIP)
            .port(PORT)
            .multithreaded()
            .concurrency(srv_threads)
            .bindaddr("0.0.0.0")
            .server_name("Valhalla")
            .run();

    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    return 0;
}

void Server::print_banner()
{
    std::srand(std::time(0));
    // Select a random color
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    fmt::color random_color = colors[std::rand() % num_colors];

    // Clean screen
    std::cout << "\033[2J\033[1;1H" << std::endl;

    // Show ASCII Art
    PRINTLINE()
    PRINT_LOGO(random_color)
    PRINTLINE()
    PRINT(" - Version", GIT_TAG, light_green, yellow)
    PRINT(" - Port", PORT, light_green, yellow)
    PRINT(" - Threads", srv_threads, light_green, yellow)
    PRINT(" - Database", fmt::format("{} {}", db_connections, "connections"), light_green, yellow)
    PRINTLINE()
    std::cout << std::endl;
}
