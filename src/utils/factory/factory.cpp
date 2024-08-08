#include "factory.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"
#include "controllers/patientcontroller/patientcontroller.hpp"
#include "controllers/providercontroller/providercontroller.hpp"
#include "controllers/servicecontroller/servicecontroller.tpp"
#include "controllers/usercontroller/usercontroller.hpp"
#include "entities/services/clinics/clinics.hpp"
#include "entities/services/laboratories.hpp"
#include "entities/services/pharmacies.hpp"
#include "entities/services/radiologycenters.hpp"
#include "store/store.hpp"
#include "utils/dosdetector/dosdetector.hpp"
#include "utils/passwordcrypt/passwordcrypt.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include "utils/tokenmanager/tokenmanager.hpp"

/**
 * @brief Initializes the application's factory, which is responsible for creating and managing various components and services used throughout the application.
 *
 * The factory registers several objects with the application's store, including:
 * - DatabaseConnectionPool: Manages a pool of database connections.
 * - DatabaseController: Provides an interface for interacting with the database.
 * - SessionManager: Manages user sessions.
 * - TokenManager: Manages authentication tokens.
 * - PasswordCrypt: Provides password hashing and encryption functionality.
 * - PatientController: Manages patient-related data and operations.
 * - ServiceControllers: Manage data and operations for various service types (clinics, pharmacies, laboratories, radiology centers).
 * - UserController: Manages user-related data and operations.
 * - ProviderController: Manages provider-related data and operations.
 * - DOSDetector: Provides functionality for detecting denial-of-service attacks.
 *
 * @param pool_size The size of the database connection pool to be created.
 */
Factory::Factory(const size_t pool_size) {
    Store::registerObject(Type::DatabaseConnectionPool, std::make_shared<DatabaseConnectionPool>(pool_size));
    Store::registerObject(Type::DatabaseController, std::make_shared<DatabaseController>());
    Store::registerObject(Type::SessionManager, std::make_shared<SessionManager>());
    Store::registerObject(Type::TokenManager, std::make_shared<TokenManager>());
    Store::registerObject(Type::PasswordCrypt, std::make_shared<PasswordCrypt>());
    Store::registerObject(Type::PatientController, std::make_shared<PatientController>());
    Store::registerObject(Type::ClinicsController, std::make_shared<ServiceController<Clinics>>());
    Store::registerObject(Type::PharmaciesController, std::make_shared<ServiceController<Pharmacies>>());
    Store::registerObject(Type::LaboratoriesController, std::make_shared<ServiceController<Laboratories>>());
    Store::registerObject(Type::RadiologyCentersController, std::make_shared<ServiceController<RadiologyCenters>>());
    Store::registerObject(Type::UserController, std::make_shared<UserController>());
    Store::registerObject(Type::ProviderController, std::make_shared<ProviderController>());
    Store::registerObject(Type::DOSDetector, std::make_shared<DOSDetector>());
}
