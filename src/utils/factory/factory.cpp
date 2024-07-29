#include "factory.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "store/store.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"

Factory::Factory(const size_t pool_size)
{
    Store::registerObject(Type::DatabaseConnectionPool, std::make_shared<DatabaseConnectionPool>(pool_size));
    Store::registerObject(Type::DatabaseController, std::make_shared<DatabaseController>());
    Store::registerObject(Type::UserSessionManager, std::make_shared<SessionManager>("users"));
    Store::registerObject(Type::ProviderSessionManager, std::make_shared<SessionManager>("providers"));
}
