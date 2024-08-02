#pragma once

#include "entities/entity.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class Pharmacies : public Entity {
private:
    std::string TABLENAME = std::string("pharmacies");

public:
    Pharmacies() : Entity(TABLENAME) {
    }

    template <typename T>
    Pharmacies(const T &_data) : Entity(_data, TABLENAME) {
    }
    std::string getTableName() {
        return TABLENAME;
    }
    ~Pharmacies() override = default;
};