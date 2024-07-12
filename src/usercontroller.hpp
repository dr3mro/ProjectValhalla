#pragma once

#include "databasecontroller.hpp"
#include "resthelper.hpp"
#include "sqlman.hpp"
#include "tokenizer.hpp"
#include <crow.h>
#include <memory>

using json = jsoncons::json;

class UserController {
public:
    UserController(std::shared_ptr<DatabaseController> dbController,
        std::shared_ptr<RestHelper> rHelper,
        std::shared_ptr<Tokenizer> tokenizer,
        std::shared_ptr<SqlMan> sqlman);
    UserController();
    ~UserController();

    // PUBLIC
    void register_user(const crow::request& req, crow::response& res);
    void login_user(const crow::request& req, crow::response& res, const jsoncons::json& credentials);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
