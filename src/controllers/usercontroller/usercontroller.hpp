#pragma once
#include "controllers/clientcontroller/clientcontroller.tpp"
#include "entities/user.hpp"
#include <memory>

// The UserController class derives from ClientController<User>
class UserController : public ClientController<User> {
public:
    UserController(const std::shared_ptr<DatabaseController>& dbController,
        const std::shared_ptr<RestHelper>& rHelper,
        const std::shared_ptr<Tokenizer>& tokenizer,
        const std::shared_ptr<PasswordCrypt>& passwordCrypt);

    // Destructor
    virtual ~UserController() = default;

    // Additional user-specific methods can be declared here
    void CreateUser(const crow::request& req, crow::response& res);
    void AuthenticateUser(crow::response& res, const jsoncons::json& credentials);
    void ReadUser(crow::response& res, const json& criteria);
    void UpdateUser(const crow::request& req, crow::response& res);
    void DeleteUser(const crow::request& req, crow::response& res, const json& criteria);
    void SearchUser(const crow::request& req, crow::response& res, const json& search_json);
};
