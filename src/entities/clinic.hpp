#pragma once

#include "entities/entity.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;
#define TABLENAME "patients"
class Clinic : public Entity {
public:
    template <typename T>
    Clinic(const T& _data)
        : Entity(_data, TABLENAME)
    {
    }

    ~Clinic() override = default;
};