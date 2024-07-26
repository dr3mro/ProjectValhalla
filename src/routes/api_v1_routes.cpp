#include "api_v1_routes.hpp"
#include <functional>
API_V1_Routes::API_V1_Routes(const std::shared_ptr<APP>& app,
    const std::shared_ptr<UserController>& userController,
    const std::shared_ptr<PatientController>& patientController,
    const std::shared_ptr<ClinicController>& clinicController)
{
    ////////////////   ROUTES   ////////////////////
    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([userController](const crow::request& req, crow::response& res) {
            userController->CreateUser(std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authentication)
        .methods(crow::HTTPMethod::GET)([userController, app](const crow::request& req, crow::response& res) {
            userController->AuthenticateUser(std::ref(res), std::cref(app->get_context<Authentication>(std::cref(req)).credentials));
        });

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
        .methods(crow::HTTPMethod::PATCH)([patientController](const crow::request& req, crow::response& res) {
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
        .methods(crow::HTTPMethod::PATCH)([clinicController](const crow::request& req, crow::response& res) {
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

    CROW_ROUTE((*app), "/v1/hello")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime)
        .methods(crow::HTTPMethod::GET)([](const crow::request& req, crow::response& res) {
            (void)req;
            res.code = crow::OK;
            res.end("{\n\"Message\" : \"Welcome to ASGARD.\"\n}");
        });

    CROW_CATCHALL_ROUTE((*app))
    ([](crow::response& res) {
        res.code = crow::NOT_FOUND;
        res.end("{\n\"Message\" : \"NOT FOUND, Are you lost?\"\n}");
    });
}
