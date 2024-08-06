#pragma once

#include "entities/base/entity.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class RadiologyCenters : public Entity {
private:
private:
    std::string TABLENAME = std::string("radiologycenters");

public:
    RadiologyCenters() : Entity(TABLENAME) {
    }

    template <typename T>
    RadiologyCenters(const T &_data) : Entity(_data, TABLENAME) {
    }
    std::string getTableName() {
        return TABLENAME;
    }
    ~RadiologyCenters() override = default;
};