#pragma once

#include "entities/base/entity.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class Laboratories : public Entity {
private:
    std::string TABLENAME = std::string("laboratories");

public:
    Laboratories() : Entity(TABLENAME) {
    }

    template <typename T>
    Laboratories(const T &_data) : Entity(_data, TABLENAME) {
    }
    std::string getTableName() {
        return TABLENAME;
    }
    ~Laboratories() override = default;
};