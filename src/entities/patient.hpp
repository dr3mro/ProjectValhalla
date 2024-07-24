#pragma once

#include "entities/entity.hpp"

class Patient : public Entity {
public:
    Patient() = default;
    ~Patient() override
    {
    }
};