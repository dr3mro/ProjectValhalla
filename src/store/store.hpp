#pragma once

#include "types.hpp"
#include <any>
#include <unordered_map>

class Store {
public:
    static void registerObject(Type type, std::any object);
    static std::any getObject(Type type);

private:
    static std::unordered_map<Type, std::any> inventory;
};

// Initialize the services map

// Register
// ServiceLocator::registerObject(Type::specialname,object);

// Retrieve
// auto dataService = ServiceLocator::getService(Type::specialname);
