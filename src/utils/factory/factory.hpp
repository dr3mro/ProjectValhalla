#pragma once
#include <cstddef>
class Factory {
public:
    explicit Factory(const size_t pool_size);
    virtual ~Factory() = default;
};