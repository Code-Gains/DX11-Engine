#pragma once
#include "Component.hpp"
class Component {
public:
    virtual ~Component() {}

    virtual void Update(const float deltaTime) {}
};