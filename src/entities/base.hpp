#pragma once

#include <optional>
#include <string>

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
