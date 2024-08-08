#pragma once
#include <cstddef>
/// A factory class that manages a pool of objects of a specific type.

class Factory {
public:
    explicit Factory(size_t pool_size);
    virtual ~Factory() = default;
};