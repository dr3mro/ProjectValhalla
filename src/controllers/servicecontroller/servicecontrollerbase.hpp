
#pragma once

#include "crow.h"
#include <cstdint>
#include <jsoncons/json.hpp>
class ServiceControllerBase {
public:
    explicit ServiceControllerBase() {
    }

    virtual ~ServiceControllerBase() {
    }

    // CRUDS
    virtual void CreateService(const crow::request &req, crow::response &res) = 0;
    virtual void ReadService(crow::response &res, const jsoncons::json &criteria) = 0;
    virtual void UpdateService(const crow::request &req, crow::response &res) = 0;
    virtual void DeleteService(crow::response &res, const jsoncons::json &delete_json) = 0;
    virtual void SearchService(crow::response &res, const jsoncons::json &search_json) = 0;

private:
    virtual uint64_t getNextID() = 0;
};