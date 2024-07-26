#pragma once
#include "controllers/controller/controller.hpp"
#include "controllers/databasecontroller/databasecontroller.hpp"

#include "utils/resthelper/resthelper.hpp"
#include "utils/tokenizer/tokenizer.hpp"
#include <crow.h>
#include <memory>

using json = jsoncons::json;

class UserController : public Controller {
public:
    explicit UserController(const std::shared_ptr<DatabaseController>& dbController,
        const std::shared_ptr<RestHelper>& rHelper,
        const std::shared_ptr<Tokenizer>& tokenizer);

    ~UserController() = default;

    // PUBLIC
    void CreateUser(const crow::request& req, crow::response& res);
    void AuthenticateUser(crow::response& res, const jsoncons::json& credentials);

private:
    std::shared_ptr<Tokenizer> tokenizer;
};
