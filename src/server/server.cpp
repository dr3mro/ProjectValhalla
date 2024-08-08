#include "server.hpp"
#include "server/extras/banner.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

#define PORT 8080
#ifndef GIT_TAG
#    define GIT_TAG "unknown"
#endif

Server::Server(const uint16_t srv_threads, const uint16_t db_connections)
    : srv_threads(srv_threads), db_connections(db_connections), app(std::make_shared<APP>()), routes(std::make_shared<API_V1_Routes>(app)) {
}

int Server::run() {
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

    } catch (const std::exception &e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    return 0;
}

void Server::print_banner() {
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
