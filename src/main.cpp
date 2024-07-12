#include "server.hpp"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    // Get the number of cpu cores of CPU to define the number of threads for the server and threadpool
    uint16_t ncpus = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1;
    uint16_t db_connections = ncpus * 5;
    uint16_t srv_threads = ncpus * 4;

    Server server(srv_threads, db_connections);

    server.run();
}