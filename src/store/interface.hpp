#pragma once

// Base interface for objects
class Object {
public:
    virtual ~Object() = default;
    virtual void Run() = 0;
};
