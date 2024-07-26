#pragma once
#include "controllers/cliniccontroller/cliniccontroller.hpp"
#include "controllers/patientcontroller/patientcontroller.hpp"
#include "controllers/usercontroller/usercontroller.hpp"
#include "middlewares/authentication.hpp"
#include "middlewares/authorization.hpp"
#include "middlewares/dataintegrity.hpp"
#include "middlewares/elapsedtime.hpp"
#include "middlewares/ratelimit.hpp"
#include "middlewares/search.hpp"
#include "middlewares/xrequest.hpp"
#include <crow.h>
#include <crow/middlewares/cors.h>
#include <memory>

using APP = crow::App<crow::CORSHandler, RateLimit, ElapsedTime, Authentication, Authorization, XRequest, Search, DataIntegrity>;

class API_V1_Routes {
public:
    API_V1_Routes(const std::shared_ptr<APP>& app,
        const std::shared_ptr<UserController>& userController,
        const std::shared_ptr<PatientController>& patientController,
        const std::shared_ptr<ClinicController>& clinicController);
};
