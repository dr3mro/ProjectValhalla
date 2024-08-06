#pragma once

#include "entities/base/entity.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class Clinics : public Entity {
private:
    static constexpr const char *TABLENAME = "clinics";

public:
    Clinics() : Entity(TABLENAME) {
    }

    template <typename T>
    Clinics(const T &_data) : Entity(_data, TABLENAME) {
    }

    std::string getTableName() {
        return TABLENAME;
    }

    ~Clinics() override = default;
};
