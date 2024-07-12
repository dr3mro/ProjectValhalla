#pragma once
#include "middlewares/authentication.hpp"
#include "middlewares/authorization.hpp"
#include "middlewares/dataintegrity.hpp"
#include "middlewares/elapsedtime.hpp"
#include "middlewares/search.hpp"
#include "patientcontroller.hpp"
#include "usercontroller.hpp"
#include <crow.h>
#include <crow/middlewares/cors.h>
#include <memory>
class API_V1_Routes {

public:
    API_V1_Routes(std::shared_ptr<crow::App<crow::CORSHandler, ElapsedTime, Authentication, Authorization, Search, DataIntegrity>> app, std::shared_ptr<UserController> userController, std::shared_ptr<PatientController> patientController);

private:
};