#include "api_v1_routes.hpp"
#include <functional>
API_V1_Routes::API_V1_Routes(const std::shared_ptr<APP>& app,
    const std::shared_ptr<UserController>& userController,
    const std::shared_ptr<PatientController>& patientController,
    const std::shared_ptr<ClinicController>& clinicController,
    const std::shared_ptr<ProviderController>& providerController)
{
    ///////////////////---------------- ROUTES --------------------////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////---------------- USERS  --------------------////////////////////
    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([userController](const crow::request& req, crow::response& res) {
            userController->CreateUser(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/ulogin")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authentication)
        .methods(crow::HTTPMethod::POST)([userController, app](const crow::request& req, crow::response& res) {
            userController->AuthenticateUser(std::ref(res), std::cref(app->get_context<Authentication>(std::cref(req)).credentials));
        });

    CROW_ROUTE((*app), "/v1/ulogout")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Deauthentication)
        .methods(crow::HTTPMethod::POST)([userController, app](const crow::request& req, crow::response& res) {
            userController->LogoutUser(std::ref(res), std::cref(app->get_context<Deauthentication>(std::cref(req)).token));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([userController, app](const crow::request& req, crow::response& res) {
            userController->ReadUser(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)([userController](const crow::request& req, crow::response& res) {
            userController->UpdateUser(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([userController, app](const crow::request& req, crow::response& res) {
            userController->DeleteUser(std::ref(req), std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([userController, app](const crow::request& req, crow::response& res) {
            userController->SearchUser(std::ref(req), std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });

    ///////////////////---------------- PROVIDERS  --------------------////////////////////
    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([providerController](const crow::request& req, crow::response& res) {
            providerController->CreateProvider(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/plogin")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authentication)
        .methods(crow::HTTPMethod::POST)([providerController, app](const crow::request& req, crow::response& res) {
            providerController->AuthenticateProvider(std::ref(res), std::cref(app->get_context<Authentication>(std::cref(req)).credentials));
        });

    CROW_ROUTE((*app), "/v1/plogout")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Deauthentication)
        .methods(crow::HTTPMethod::POST)([providerController, app](const crow::request& req, crow::response& res) {
            providerController->LogoutUser(std::ref(res), std::cref(app->get_context<Deauthentication>(std::cref(req)).token));
        });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([providerController, app](const crow::request& req, crow::response& res) {
            providerController->ReadProvider(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)([providerController](const crow::request& req, crow::response& res) {
            providerController->UpdateProvider(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([providerController, app](const crow::request& req, crow::response& res) {
            providerController->DeleteProvider(std::ref(req), std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([providerController, app](const crow::request& req, crow::response& res) {
            providerController->SearchProvider(std::ref(req), std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });

    ///////////////////---------------- PATIENT --------------------////////////////////
    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([patientController](const crow::request& req, crow::response& res) {
            patientController->CreatePatient(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([patientController, app](const crow::request& req, crow::response& res) {
            patientController->ReadPatient(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)([patientController](const crow::request& req, crow::response& res) {
            patientController->UpdatePatient(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([patientController, app](const crow::request& req, crow::response& res) {
            patientController->DeletePatient(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([patientController, app](const crow::request& req, crow::response& res) {
            patientController->SearchPatient(std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });
    ///////////////////---------------- CLINIC  --------------------////////////////////
    CROW_ROUTE((*app), "/v1/clinic")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([clinicController](const crow::request& req, crow::response& res) {
            clinicController->CreateClinic(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/clinic")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([clinicController, app](const crow::request& req, crow::response& res) {
            clinicController->ReadClinic(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/clinic")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)([clinicController](const crow::request& req, crow::response& res) {
            clinicController->UpdateClinic(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/clinic")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([clinicController, app](const crow::request& req, crow::response& res) {
            clinicController->DeleteClinic(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/clinic")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([clinicController, app](const crow::request& req, crow::response& res) {
            clinicController->SearchClinic(std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });
    ///////////////////---------------- HELLO  --------------------////////////////////
    CROW_ROUTE((*app), "/v1/hello")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime)
        .methods(crow::HTTPMethod::GET)([](const crow::request& req, crow::response& res) {
            (void)req;
            res.code = crow::OK;
            res.end("{\n\"Message\" : \"Welcome to ASGARD.\"\n}");
        });
    ///////////////////---------------- CATCHALL  --------------------////////////////////
    CROW_CATCHALL_ROUTE((*app))
    ([](crow::response& res) {
        res.code = crow::NOT_FOUND;
        res.end("{\n\"Message\" : \"NOT FOUND, Are you lost?\"\n}");
    });
}
