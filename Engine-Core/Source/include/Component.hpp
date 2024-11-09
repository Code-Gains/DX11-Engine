#pragma once
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <bitset>
#include <functional>
#include <optional>
#include <any>
#include "Entity.hpp"

constexpr size_t MAX_COMPONENTS = 64;
using ComponentSignature = std::bitset<MAX_COMPONENTS>;
using ComponentType = std::uint16_t;

class Component
{
public:
    virtual ~Component() = default;
};

class ComponentUI
{
public:
    ComponentUI() = default;
    virtual ~ComponentUI() = default;
    virtual void RenderUI(Component& component) = 0;
};

class IComponentVector
{
public:
    virtual ~IComponentVector() = default;
    virtual void* GetComponent(Entity entity) = 0;
    virtual void CloneComponent(Entity entity, IComponentVector& target) const = 0;
    virtual void TransferComponent(Entity entity, IComponentVector& target) const = 0;
    virtual void RemoveComponent(Entity entity) = 0;
    virtual size_t GetComponentCount() const = 0;
};

template<typename TComponent>
class ComponentVector : public IComponentVector
{
    std::vector<TComponent> _components;
    std::unordered_map<Entity, size_t> _entityToIndex;

public:
    void* GetComponent(Entity entity)
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
            return &(_components[it->second]);

        throw std::runtime_error("Component not found for entity!");
    }

    /*void GetComponent(Entity entity) const override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
        {
            TComponent const& component = _components[it->second];
            return static_cast<ComponentVector<TComponent>&>(component);
        }
    }*/

    size_t GetComponentCount() const override
    {
        return _components.size();
    }

    // RETURN REF TODO
    std::vector<TComponent>* GetRawVector()
    {
        return &_components;
    }

    const std::vector<TComponent>& GetRawVectorConst() const
    {
        return _components;
    }

    const std::unordered_map<Entity, size_t>& GetEntityToIndex() const
    {
        return _entityToIndex;
    }

    void CloneComponent(Entity entity, IComponentVector& target) const override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
        {
            TComponent const& component = _components[it->second];
            static_cast<ComponentVector<TComponent>&>(target).AddComponent(entity, component);
        }
    }

    void TransferComponent(Entity entity, IComponentVector& target) const override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
        {
            TComponent const& component = _components[it->second];
            static_cast<ComponentVector<TComponent>&>(target).AddComponent(entity, component);

            const_cast<ComponentVector<TComponent>*>(this)->RemoveComponent(entity);
        }
    }

    void AddComponent(Entity entity, const TComponent& component)
    {
        _components.push_back(component);
        _entityToIndex[entity] = _components.size() - 1;
    }

    void RemoveComponent(Entity entity) override
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end()) {
            size_t indexToRemove = it->second;

            // if it's not the last element, swap with the last one for fast removal
            if (indexToRemove != _components.size() - 1) {
                std::swap(_components[indexToRemove], _components.back());
                // update the swapped component's index in _entityToIndex TODO simplify
                auto entityOfLastComponent = std::find_if(_entityToIndex.begin(), _entityToIndex.end(),
                    [this, indexToRemove](const auto& pair) {
                        return pair.second == _components.size() - 1;
                    })->first;
                _entityToIndex[entityOfLastComponent] = indexToRemove;
            }
            _components.pop_back();
            _entityToIndex.erase(it);
        }
    }

public:
    ComponentVector() {};
    ~ComponentVector() {};
};

class IComponentVectorFactory
{
public:
    virtual ~IComponentVectorFactory();

};
