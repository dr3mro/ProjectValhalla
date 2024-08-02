#include "factory.hpp"
#include "controllers/cliniccontroller/cliniccontroller.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
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
#include "middlewares/xrequest.hpp"
#include "routes/api_v1_routes.hpp"
#include "store/store.hpp"
#include "utils/dosdetector/dosdetector.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include "utils/resthelper/resthelper.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"

Factory::Factory(const size_t pool_size)
{
    Store::registerObject(Type::DatabaseConnectionPool, std::make_shared<DatabaseConnectionPool>(pool_size));
    Store::registerObject(Type::DatabaseController, std::make_shared<DatabaseController>());
    Store::registerObject(Type::RestHelper, std::make_shared<RestHelper>());
    Store::registerObject(Type::SessionManager, std::make_shared<SessionManager>());
    Store::registerObject(Type::TokenManager, std::make_shared<TokenManager>());
    Store::registerObject(Type::PasswordCrypt, std::make_shared<PasswordCrypt>());
    Store::registerObject(Type::PatientController, std::make_shared<PatientController>());
    Store::registerObject(Type::ClinicController, std::make_shared<ClinicController>());
    Store::registerObject(Type::UserController, std::make_shared<UserController>());
    Store::registerObject(Type::ProviderController, std::make_shared<ProviderController>());
    Store::registerObject(Type::DOSDetector, std::make_shared<DOSDetector>());
}
