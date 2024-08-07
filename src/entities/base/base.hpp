#pragma once

#include <optional>
#include <string>

/// Base class for all entities in the application.
/// Provides common functionality for creating, reading, updating, deleting, and searching entities.
/// Derived classes must implement the virtual functions to provide the SQL statements for each operation.
class Base {
public:
    explicit Base() = default;
    virtual ~Base() = default;

    virtual std::optional<std::string> getSqlCreateStatement() = 0;
    virtual std::optional<std::string> getSqlReadStatement() = 0;
    virtual std::optional<std::string> getSqlUpdateStatement() = 0;
    virtual std::optional<std::string> getSqlDeleteStatement() = 0;
    virtual std::optional<std::string> getSqlSearchStatement() = 0;
};
