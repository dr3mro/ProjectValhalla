#pragma once

#include <string>

class Entity {
public:
    explicit Entity() = default;
    virtual ~Entity() = default;

    virtual std::string getSqlCreateStatement() = 0;
    virtual std::string getSqlReadStatement() = 0;
    virtual std::string getSqlUpdateStatement() = 0;
    virtual std::string getSqlDeleteStatement() = 0;
    virtual std::string getSqlSearchStatement() = 0;
};
