#pragma once
#include "interface.hpp"
#include <iostream>

// Specific object implementations
class Database : public Object {
public:
    void Run() override
    {
        std::cout << "Performing Database Action" << std::endl;
    }
};
