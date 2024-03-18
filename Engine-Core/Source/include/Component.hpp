#pragma once
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <bitset>
#include <functional>
#include <optional>
#include "Entity.hpp"

constexpr size_t MAX_COMPONENTS = 64;
using ComponentSignature = std::bitset<MAX_COMPONENTS>;
using ComponentType = std::uint16_t;

class IComponent
{
public:
    virtual ~IComponent() = default;
};

class IComponentVector
{
public:
    virtual ~IComponentVector() = default;
    //virtual void GetComponent(Entity entity) const = 0;
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
    /*TComponent& GetComponent(Entity entity)
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
            return _components[it->second];

        throw std::runtime_error("Component not found for entity!");
    }*/

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

class ComponentRegistry {
private:
    // max of 2^16 component types
    static ComponentType _nextComponentType;
    static std::unordered_map<std::type_index, ComponentType> _componentTypes;
    static std::unordered_map<std::type_index, std::string> _componentNames;
    static std::unordered_map<ComponentType, std::function<std::unique_ptr<IComponentVector>()>> _vectorFactories;

public:

    template<typename TComponent>
    static std::optional<ComponentType> GetComponentType() 
    {
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from Component!");

        auto type = std::type_index(typeid(TComponent));
        if (_componentTypes.find(type) == _componentTypes.end())
            return std::nullopt;

        return _componentTypes[type];
    }

    template<typename... TComponents>
    static std::optional<ComponentSignature> GetComponentArraySignature()
    {
        ComponentSignature signature;

        // &signature captures the local variable and places it inside the scope
        auto processComponent = [&signature](auto compTypeOpt) -> bool
        {
            if (!compTypeOpt)
                return false;

            // set the bit in the signature bitset
            signature.set(*compTypeOpt);
            return true;
        };

        // call GetComponentType for each value in TComponents and accumulate validations with &&
        bool valid = (processComponent(GetComponentType<TComponents>()) && ...);

        if (!valid) // return even if at least one component was failed to be identified
            return std::nullopt;

        // bitset was generated in processComponent
        return signature;
    }

    template<typename TComponent>
    static ComponentType GetOrRegisterComponentType()
    {
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from Component!");

        auto type = std::type_index(typeid(TComponent));
        if (_componentTypes.find(type) == _componentTypes.end())
        {
            ComponentType ct = _nextComponentType++;
            _componentTypes[type] = ct;
            _componentNames[type] = typeid(TComponent).name();
            _vectorFactories[ct] = []() -> std::unique_ptr<IComponentVector>
            {
                // this does not return, just saves the expression into std::function
                return std::make_unique<ComponentVector<TComponent>>();
            };
            return ct;
        }
        return _componentTypes[type];
    }

    static std::function<std::unique_ptr<IComponentVector>()> GetFactoryFunction(ComponentType componentType)
    {
        auto it = _vectorFactories.find(componentType);
        if (it != _vectorFactories.end())
        {
            return it->second;
        }

        throw std::runtime_error("Could not find a factory for component vector");
    }

    static size_t GetComponentCount()
    {
        return _nextComponentType;
    }
};
