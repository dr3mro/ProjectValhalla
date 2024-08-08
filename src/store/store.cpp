
#include "store.hpp"

/// The inventory of objects stored in the Store.
/// This is a mapping from object types to their corresponding objects.
std::unordered_map<Type, std::any> Store::inventory;

void Store::registerObject(Type type, std::any object)
{
    inventory[type] = std::move(object);
}

/// Retrieves the object of the given type from the store's inventory.
///
/// @param type The type of the object to retrieve.
/// @return The object of the given type, or `nullptr` if it is not found in the inventory.
std::any Store::getObject(Type type)
{
    auto it = inventory.find(type);
    if (it != inventory.end()) {
        return it->second;
    }
    return nullptr;
}
