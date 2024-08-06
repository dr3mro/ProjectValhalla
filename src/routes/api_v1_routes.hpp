#pragma once
#include <crow.h>
#include <crow/middlewares/cors.h>

#include <memory>
#include <typeindex>

#include "controllers/patientcontroller/patientcontroller.hpp"
#include "controllers/providercontroller/providercontroller.hpp"
#include "controllers/servicecontroller/servicecontroller.tpp"
#include "controllers/usercontroller/usercontroller.hpp"
#include "entities/services/clinics/clinics.hpp"
#include "entities/services/laboratories.hpp"
#include "entities/services/pharmacies.hpp"
#include "entities/services/radiologycenters.hpp"
#include "middlewares/authentication.hpp"
#include "middlewares/authorization.hpp"
#include "middlewares/dataintegrity.hpp"
#include "middlewares/deauthentication.hpp"
#include "middlewares/elapsedtime.hpp"
#include "middlewares/ratelimit.hpp"
#include "middlewares/search.hpp"

using APP = crow::App<crow::CORSHandler, RateLimit, ElapsedTime, Authentication, Deauthentication, Authorization, XRequest, Search, DataIntegrity>;

class API_V1_Routes {
public:
    API_V1_Routes(std::shared_ptr<APP> &app);
    ~API_V1_Routes() = default;

private:
    std::shared_ptr<UserController> userController;
    std::shared_ptr<ProviderController> providerController;
    std::shared_ptr<PatientController> patientController;
    std::shared_ptr<ServiceController<Clinics>> clinicsController;
    std::shared_ptr<ServiceController<Pharmacies>> pharmaciesController;
    std::shared_ptr<ServiceController<Laboratories>> laboratoriesController;
    std::shared_ptr<ServiceController<RadiologyCenters>> radiologyCentersController;


    // Type mapping to determine the correct type from the string
    std::map<std::string, std::type_index> typeMapping = {
        {          "clinics",          typeid(ServiceController<Clinics>) },
        {       "pharmacies",       typeid(ServiceController<Pharmacies>) },
        {     "laboratories",     typeid(ServiceController<Laboratories>) },
        { "radiologycenters", typeid(ServiceController<RadiologyCenters>) }
    };

    std::map<std::string, Type> serviceControllerMap = {
        {          "clinics",          Type::ClinicsController },
        {       "pharmacies",       Type::PharmaciesController },
        {     "laboratories",     Type::LaboratoriesController },
        { "radiologycenters", Type::RadiologyCentersController }
    };

    template <typename Func, typename... Args>
    /**
     * Executes a service method on the appropriate service controller based on the provided service name.
     *
     * This function is responsible for finding the correct service controller based on the provided service name, and then invoking the specified method on
     * that controller. It handles type-safe casting of the controller to the appropriate type, and provides error handling for cases where the service name is
     * not found or the type mapping is not found.
     *
     * @param serviceName The name of the service to execute the method on.
     * @param method The method to invoke on the service controller.
     * @param req The Crow request object.
     * @param res The Crow response object.
     * @param args The arguments to pass to the service controller method.
     */
    void executeServiceMethod(const std::string &serviceName, Func method, const crow::request &req, crow::response &res, Args &&...args) {

        auto it = serviceControllerMap.find(serviceName);

        if (it != serviceControllerMap.end()) {
            auto typeIt = typeMapping.find(serviceName);
            if (typeIt != typeMapping.end()) {
                std::type_index typeIndex = typeIt->second;
                try {
                    if (typeIndex == typeid(ServiceController<Clinics>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<Clinics>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::cref(req), std::ref(res), std::forward<Args>(args)...);
                    } else if (typeIndex == typeid(ServiceController<Pharmacies>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<Pharmacies>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::cref(req), std::ref(res), std::forward<Args>(args)...);
                    } else if (typeIndex == typeid(ServiceController<Laboratories>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<Laboratories>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::cref(req), std::ref(res), std::forward<Args>(args)...);
                    } else if (typeIndex == typeid(ServiceController<RadiologyCenters>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<RadiologyCenters>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::cref(req), std::ref(res), std::forward<Args>(args)...);
                    } else {
                        std::cerr << "Unsupported service type for: " << serviceName << std::endl;
                    }
                } catch (const std::bad_any_cast &e) {
                    std::cerr << "Bad cast: " << e.what() << std::endl;
                    res.code = 500;
                    res.body = "Internal Server Error";
                    res.end();
                }
            } else {
                std::cerr << "Type mapping not found for: " << serviceName << std::endl;
                res.code = 400;
                res.body = "Type mapping not found";
                res.end();
            }
        } else {
            std::cerr << "Service not found: " << serviceName << std::endl;
            res.code = 404;
            res.body = "Service not found";
            res.end();
        }
    }
};