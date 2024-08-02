#include "api_v1_routes.hpp"

#include <functional>
API_V1_Routes::API_V1_Routes(std::shared_ptr<APP> &app) {
    userController = std::any_cast<std::shared_ptr<UserController>>(Store::getObject(Type::UserController));
    providerController = std::any_cast<std::shared_ptr<ProviderController>>(Store::getObject(Type::ProviderController));
    patientController = std::any_cast<std::shared_ptr<PatientController>>(Store::getObject(Type::PatientController));
    clinicsController = std::any_cast<std::shared_ptr<ServiceController<Clinics>>>(Store::getObject(Type::ClinicsController));
    pharmaciesController = std::any_cast<std::shared_ptr<ServiceController<Pharmacies>>>(Store::getObject(Type::PharmaciesController));
    laboratoriesController = std::any_cast<std::shared_ptr<ServiceController<Laboratories>>>(Store::getObject(Type::LaboratoriesController));
    radiologyCentersController = std::any_cast<std::shared_ptr<ServiceController<RadiologyCenters>>>(Store::getObject(Type::RadiologyCentersController));


    ///////////////////---------------- ROUTES--------------------////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    ///////////////////---------------- USERS--------------------////////////////////
    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([this](const crow::request &req, crow::response &res) { userController->CreateUser(std::cref(req), std::ref(res)); });

    CROW_ROUTE((*app), "/v1/ulogin")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authentication)
        .methods(crow::HTTPMethod::POST)([this, app](const crow::request &req, crow::response &res) {
            userController->AuthenticateUser(std::ref(res), std::cref(app->get_context<Authentication>(std::cref(req)).credentials));
        });

    CROW_ROUTE((*app), "/v1/ulogout")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Deauthentication)
        .methods(crow::HTTPMethod::POST)([this, app](const crow::request &req, crow::response &res) {
            userController->LogoutUser(std::ref(res), std::cref(app->get_context<Deauthentication>(std::cref(req)).token));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([this, app](const crow::request &req, crow::response &res) {
            userController->ReadUser(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)([this](const crow::request &req, crow::response &res) { userController->UpdateUser(std::cref(req), std::ref(res)); });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([this, app](const crow::request &req, crow::response &res) {
            userController->DeleteUser(std::ref(req), std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([this, app](const crow::request &req, crow::response &res) {
            userController->SearchUser(std::ref(req), std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });

    ///////////////////---------------- PROVIDERS
    ///--------------------////////////////////
    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, DataIntegrity)
        .methods(crow::HTTPMethod::POST)(
            [this](const crow::request &req, crow::response &res) { providerController->CreateProvider(std::cref(req), std::ref(res)); });

    CROW_ROUTE((*app), "/v1/plogin")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authentication)
        .methods(crow::HTTPMethod::POST)([this, app](const crow::request &req, crow::response &res) {
            providerController->AuthenticateProvider(std::ref(res), std::cref(app->get_context<Authentication>(std::cref(req)).credentials));
        });

    CROW_ROUTE((*app), "/v1/plogout")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Deauthentication)
        .methods(crow::HTTPMethod::POST)([this, app](const crow::request &req, crow::response &res) {
            providerController->LogoutUser(std::ref(res), std::cref(app->get_context<Deauthentication>(std::cref(req)).token));
        });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([this, app](const crow::request &req, crow::response &res) {
            providerController->ReadProvider(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)(
            [this](const crow::request &req, crow::response &res) { providerController->UpdateProvider(std::cref(req), std::ref(res)); });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([this, app](const crow::request &req, crow::response &res) {
            providerController->DeleteProvider(std::ref(req), std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/provider")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([this, app](const crow::request &req, crow::response &res) {
            providerController->SearchProvider(std::ref(req), std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });

    ///////////////////---------------- PATIENT
    ///--------------------////////////////////
    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::POST)(
            [this](const crow::request &req, crow::response &res) { patientController->CreatePatient(std::cref(req), std::ref(res)); });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([this, app](const crow::request &req, crow::response &res) {
            patientController->ReadPatient(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)(
            [this](const crow::request &req, crow::response &res) { patientController->UpdatePatient(std::cref(req), std::ref(res)); });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([this, app](const crow::request &req, crow::response &res) {
            patientController->DeletePatient(std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([this, app](const crow::request &req, crow::response &res) {
            patientController->SearchPatient(std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });
    ///////////////////---------------- Service --------------------////////////////////
    CROW_ROUTE((*app), "/v1/service/<string>")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([this](const crow::request &req, crow::response &res, const std::string &serviceName) {
            executeServiceMethod(serviceName, &ServiceControllerBase::CreateService, req, res);
        });

    CROW_ROUTE((*app), "/v1/service/<string>")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([this, app](const crow::request &req, crow::response &res, const std::string &serviceName) {
            executeServiceMethod(serviceName, &ServiceControllerBase::ReadService, std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/service/<string>")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PUT)([this](const crow::request &req, crow::response &res, const std::string &serviceName) {
            executeServiceMethod(serviceName, &ServiceControllerBase::UpdateService, std::cref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/service/<string>")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([this, app](const crow::request &req, crow::response &res, const std::string &serviceName) {
            executeServiceMethod(
                serviceName, &ServiceControllerBase::DeleteService, std::ref(res), std::cref(app->get_context<XRequest>(std::cref(req)).criteria));
        });

    CROW_ROUTE((*app), "/v1/service/<string>")
        .CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([this, app](const crow::request &req, crow::response &res, const std::string &serviceName) {
            executeServiceMethod(
                serviceName, &ServiceControllerBase::SearchService, std::ref(res), std::cref(app->get_context<Search>(std::cref(req)).search_json));
        });

    ///////////////////---------------- HELLO--------------------////////////////////
    CROW_ROUTE((*app), "/v1/hello").CROW_MIDDLEWARES(*app, RateLimit, ElapsedTime).methods(crow::HTTPMethod::GET)([](const crow::request &req, crow::response &res) {
        (void) req;
        res.code = crow::OK;
        res.end("{\n\"Message\" : \"Welcome to ASGARD.\"\n}");
    });
}
