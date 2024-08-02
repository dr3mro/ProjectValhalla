#pragma once
#include "controllers/cliniccontroller/cliniccontroller.hpp"
#include "controllers/patientcontroller/patientcontroller.hpp"
#include "controllers/providercontroller/providercontroller.hpp"
#include "controllers/usercontroller/usercontroller.hpp"
#include "middlewares/authentication.hpp"
#include "middlewares/authorization.hpp"
#include "middlewares/dataintegrity.hpp"
#include "middlewares/deauthentication.hpp"
#include "middlewares/elapsedtime.hpp"
#include "middlewares/ratelimit.hpp"
#include "middlewares/search.hpp"
#include <crow.h>
#include <crow/middlewares/cors.h>
#include <memory>

using APP = crow::App<crow::CORSHandler, RateLimit, ElapsedTime, Authentication, Deauthentication, Authorization, XRequest, Search, DataIntegrity>;

class API_V1_Routes {
public:
    API_V1_Routes(std::shared_ptr<APP>& app);
    ~API_V1_Routes() = default;

private:
    // std::shared_ptr<RateLimit> rateLimit = std::make_shared<RateLimit>(); //
    // std::shared_ptr<ElapsedTime> elapsedTime = std::make_shared<ElapsedTime>();
    // std::shared_ptr<Authentication> authentication = std::make_shared<Authentication>();
    // std::shared_ptr<Authorization> authorization = std::make_shared<Authorization>();
    // std::shared_ptr<Deauthentication> deauthentication = std::make_shared<Deauthentication>(); //
    // std::shared_ptr<DataIntegrity> dataIntegrity = std::make_shared<DataIntegrity>(); //
    // std::shared_ptr<Search> search = std::make_shared<Search>(); //
    // std::shared_ptr<XRequest> xrequest = std::make_shared<XRequest>(); //

    // std::shared_ptr<APP> app = std::make_shared<APP>(*rateLimit, *elapsedTime, *authentication, *authorization, *deauthentication, *xrequest, *search, *dataIntegrity);

    std::shared_ptr<UserController> userController;
    std::shared_ptr<ProviderController> providerController;
    std::shared_ptr<PatientController> patientController;
    std::shared_ptr<ClinicController> clinicController;
};
