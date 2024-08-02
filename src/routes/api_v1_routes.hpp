#pragma once
#include <crow.h>
#include <crow/middlewares/cors.h>

#include <memory>
#include <typeindex>

#include "controllers/patientcontroller/patientcontroller.hpp"
#include "controllers/providercontroller/providercontroller.hpp"
#include "controllers/servicecontroller/servicecontroller.tpp"
#include "controllers/usercontroller/usercontroller.hpp"
#include "entities/clinics.hpp"
#include "entities/laboratories.hpp"
#include "entities/pharmacies.hpp"
#include "entities/radiologycenters.hpp"
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
    void executeServiceMethod(const std::string &serviceName, Func method, Args &&...args) {
        auto it = serviceControllerMap.find(serviceName);
        if (it != serviceControllerMap.end()) {
            auto typeIt = typeMapping.find(serviceName);
            if (typeIt != typeMapping.end()) {
                std::type_index typeIndex = typeIt->second;
                try {
                    if (typeIndex == typeid(ServiceController<Clinics>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<Clinics>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::forward<Args>(args)...);
                    } else if (typeIndex == typeid(ServiceController<Pharmacies>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<Pharmacies>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::forward<Args>(args)...);
                    } else if (typeIndex == typeid(ServiceController<Laboratories>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<Laboratories>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::forward<Args>(args)...);
                    } else if (typeIndex == typeid(ServiceController<RadiologyCenters>)) {
                        auto controller = std::any_cast<std::shared_ptr<ServiceController<RadiologyCenters>>>(Store::getObject(it->second));
                        std::invoke(method, *controller, std::forward<Args>(args)...);
                    } else {
                        std::cerr << "Unsupported service type for: " << serviceName << std::endl;
                    }
                } catch (const std::bad_any_cast &e) {
                    std::cerr << "Bad cast: " << e.what() << std::endl;
                }
            } else {
                std::cerr << "Type mapping not found for: " << serviceName << std::endl;
            }
        } else {
            std::cerr << "Service not found: " << serviceName << std::endl;
        }
    }
};