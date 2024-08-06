#pragma once
#include "controllers/base/clientcontroller/clientcontroller.tpp"
#include "entities/people/user.hpp"
#include "utils/sessionmanager/sessionmanager.hpp"
#include <memory>

// The UserController class derives from ClientController<User>
class UserController : public ClientController<User> {
public:
    UserController();

    // Destructor
    ~UserController() override = default;

    // Additional user-specific methods can be declared here
    void CreateUser(const crow::request &req, crow::response &res);
    void AuthenticateUser(crow::response &res, const jsoncons::json &credentials);
    void ReadUser(crow::response &res, const json &criteria);
    void UpdateUser(const crow::request &req, crow::response &res);
    void DeleteUser(const crow::request &req, crow::response &res, const json &criteria);
    void SearchUser(const crow::request &req, crow::response &res, const json &search_json);
    void LogoutUser(crow::response &res, const std::optional<std::string> &token);

private:
    std::shared_ptr<SessionManager> sessionManager;
};
