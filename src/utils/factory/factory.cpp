#include "factory.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "store/store.hpp"
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
}
