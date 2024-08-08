#pragma once

// Enum for object types
/// Enum representing the different types of objects that can be managed by the application's store.
///
/// - `DatabaseConnectionPool`: Represents a pool of database connections.
/// - `DatabaseController`: Handles interactions with the database.
/// - `SessionManager`: Manages user sessions.
/// - `TokenManager`: Handles authentication tokens.
/// - `PasswordCrypt`: Provides password encryption and hashing functionality.
/// - `PatientController`: Manages patient-related data and operations.
/// - `ClinicsController`: Manages clinic-related data and operations.
/// - `PharmaciesController`: Manages pharmacy-related data and operations.
/// - `LaboratoriesController`: Manages laboratory-related data and operations.
/// - `RadiologyCentersController`: Manages radiology center-related data and operations.
/// - `UserController`: Manages user-related data and operations.
/// - `ProviderController`: Manages provider-related data and operations.
/// - `DOSDetector`: Detects denial-of-service (DoS) attacks.
enum class Type {
    DatabaseConnectionPool,
    DatabaseController,
    SessionManager,
    TokenManager,
    PasswordCrypt,
    PatientController,
    ClinicsController,
    PharmaciesController,
    LaboratoriesController,
    RadiologyCentersController,
    UserController,
    ProviderController,
    DOSDetector
};
