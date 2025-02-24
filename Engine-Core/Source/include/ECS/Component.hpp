#pragma once
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <bitset>
#include <functional>
#include <optional>
#include <any>

#include <cereal/cereal.hpp>

#include "Entity.hpp"

constexpr size_t MAX_COMPONENTS = 64;
using ComponentSignature = std::bitset<MAX_COMPONENTS>;
using ComponentType = std::uint16_t;

class Component
{
public:
    virtual ~Component() = default;

    template <class Archive>
    void save(Archive& archive) const
    {

    }

    template <class Archive>
    void load(Archive& archive) const
    {

    }
};

class ComponentUI
{
public:
    ComponentUI() = default;
    virtual ~ComponentUI() = default;
    virtual void RenderUI(Component& component) = 0;
};