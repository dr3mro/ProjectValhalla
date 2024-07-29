
#include "store.hpp"

std::unordered_map<Type, std::any> Store::inventory;

void Store::registerObject(Type type, std::any object)
{
    inventory[type] = std::move(object);
}

std::any Store::getObject(Type type)
{
    auto it = inventory.find(type);
    if (it != inventory.end()) {
        return it->second;
    }
    return nullptr;
}
