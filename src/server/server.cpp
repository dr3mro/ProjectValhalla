#include "server.hpp"
#include "server/extras/banner.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

#define PORT 8080
#ifndef GIT_TAG
#define GIT_TAG "unknown"
#endif

Server::Server(uint16_t _srv_threads, uint16_t _db_connections)

{
    srv_threads = _srv_threads;
    db_connections = _db_connections;

    dbConnPool = std::make_shared<DatabaseConnectionPool>(db_connections);
    dbController = std::make_shared<DatabaseController>();
    restHelper = std::make_shared<RestHelper>(dbController);
    tokenManager = std::make_shared<TokenManager>();
    passwordCrypt = std::make_shared<PasswordCrypt>();
    patientController = std::make_shared<PatientController>(dbController, restHelper);
    clinicController = std::make_shared<ClinicController>(dbController, restHelper);
    userController = std::make_shared<UserController>(dbController, restHelper, tokenManager, passwordCrypt);
    providerController = std::make_shared<ProviderController>(dbController, restHelper, tokenManager, passwordCrypt);
    dos_detector = std::make_shared<DOSDetector>();
    rateLimit = std::make_shared<RateLimit>(dos_detector);
    elapsedTime = std::make_shared<ElapsedTime>();
    authentication = std::make_shared<Authentication>();
    authorization = std::make_shared<Authorization>(tokenManager);
    deauthentication = std::make_shared<Deauthentication>();
    dataIntegrity = std::make_shared<DataIntegrity>();
    search = std::make_shared<Search>();
    xrequest = std::make_shared<XRequest>();
    app = std::make_shared<APP>(*rateLimit, *elapsedTime, *authentication, *authorization, *deauthentication, *xrequest, *search, *dataIntegrity);
    routes = std::make_shared<API_V1_Routes>(app, userController, patientController, clinicController, providerController);
}

int Server::run()
{
    print_banner();

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
