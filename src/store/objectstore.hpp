#pragma once

#include <unordered_map>
#include <utility>

#include "interface.hpp"
#include "objecttypes.hpp"

class ObjectStore {
public:
    static void registerObject(ObjectType type, std::shared_ptr<Object> object)
    {
        inventory[type] = std::move(object);
    }

    static std::shared_ptr<Object> getObject(ObjectType type)
    {
        auto it = inventory.find(type);
        if (it != inventory.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    static std::unordered_map<ObjectType, std::shared_ptr<Object>> inventory;
};

// Initialize the services map

// Register services
// ServiceLocator::registerService(ServiceType::DataService, std::make_shared<DataService>());

// Retrieve and use services
// auto dataService = ServiceLocator::getService(ServiceType::DataService);
// if (dataService)
//    dataService->Run();