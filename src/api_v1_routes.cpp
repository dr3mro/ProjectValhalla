#include "api_v1_routes.hpp"

API_V1_Routes::API_V1_Routes(std::shared_ptr<crow::App<crow::CORSHandler, ElapsedTime, Authentication, Authorization, XRequest, Search, DataIntegrity>> app, std::shared_ptr<UserController> userController, std::shared_ptr<PatientController> patientController)
{
    ////////////////   ROUTES   ////////////////////
    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, ElapsedTime, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([userController](const crow::request& req, crow::response& res) {
            userController->register_user(std::ref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/user")
        .CROW_MIDDLEWARES(*app, ElapsedTime, Authentication)
        .methods(crow::HTTPMethod::GET)([userController, app](const crow::request& req, crow::response& res) {
            userController->login_user(std::ref(req), std::ref(res), app->get_context<Authentication>(req).credentials);
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::POST)([patientController](const crow::request& req, crow::response& res) {
            patientController->create_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, ElapsedTime, Authorization, XRequest)
        .methods(crow::HTTPMethod::GET)([patientController, app](const crow::request& req, crow::response& res) {
            patientController->read_patient(std::ref(req), std::ref(res), app->get_context<XRequest>(req).criteria);
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, ElapsedTime, Authorization, DataIntegrity)
        .methods(crow::HTTPMethod::PATCH)([patientController](const crow::request& req, crow::response& res) {
            patientController->update_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, ElapsedTime, Authorization, XRequest, DataIntegrity)
        .methods(crow::HTTPMethod::DELETE)([patientController, app](const crow::request& req, crow::response& res) {
            patientController->delete_patient(std::ref(req), std::ref(res), app->get_context<XRequest>(req).criteria);
        });

    CROW_ROUTE((*app), "/v1/patient")
        .CROW_MIDDLEWARES(*app, ElapsedTime, Authorization, XRequest, Search)
        .methods(crow::HTTPMethod::SEARCH)([patientController, app](const crow::request& req, crow::response& res) {
            patientController->search_patient(std::ref(req), std::ref(res), app->get_context<Search>(req).search_json);
        });

    CROW_ROUTE((*app), "/v1/hello")
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
