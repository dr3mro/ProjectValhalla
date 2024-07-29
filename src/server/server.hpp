#pragma once
#include "controllers/cliniccontroller/cliniccontroller.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "controllers/patientcontroller/patientcontroller.hpp"
#include "controllers/providercontroller/providercontroller.hpp"
#include "controllers/usercontroller/usercontroller.hpp"
#include "database/databaseconnectionpool.hpp"
#include "middlewares/authentication.hpp"
#include "middlewares/authorization.hpp"
#include "middlewares/dataintegrity.hpp"
#include "middlewares/elapsedtime.hpp"
#include "middlewares/ratelimit.hpp"
#include "middlewares/search.hpp"
#include "middlewares/xrequest.hpp"
#include "routes/api_v1_routes.hpp"
#include "utils/dosdetector/dosdetector.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"
#include <crow.h>
#include <memory>

class Server {
public:
    Server(uint16_t srv_threads, uint16_t db_connections);
    ~Server() = default;

    int run();

private:
    std::shared_ptr<DatabaseConnectionPool> dbConnPool;
    std::shared_ptr<DatabaseController> dbController;
    std::shared_ptr<RestHelper> restHelper;
    std::shared_ptr<TokenManager> tokenManager;
    std::shared_ptr<PasswordCrypt> passwordCrypt;
    std::shared_ptr<PatientController> patientController;
    std::shared_ptr<ClinicController> clinicController;
    std::shared_ptr<UserController> userController;
    std::shared_ptr<ProviderController> providerController;
    std::shared_ptr<DOSDetector> dos_detector;
    std::shared_ptr<RateLimit> rateLimit;
    std::shared_ptr<ElapsedTime> elapsedTime;
    std::shared_ptr<Authentication> authentication;
    std::shared_ptr<Authorization> authorization;
    std::shared_ptr<Deauthentication> deauthentication;
    std::shared_ptr<DataIntegrity> dataIntegrity;
    std::shared_ptr<Search> search;
    std::shared_ptr<XRequest> xrequest;
    std::shared_ptr<APP> app;
    std::shared_ptr<API_V1_Routes> routes;
    uint16_t srv_threads;
    uint16_t db_connections;
    void print_banner();
};