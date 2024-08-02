#pragma once

#include "entities/entity.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

#define TABLENAME "patients"
class Patient : public Entity {
public:
  template <typename T> Patient(const T &_data) : Entity(_data, TABLENAME) {}

  ~Patient() override = default;
};