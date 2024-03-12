#pragma once
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <bitset>
#include "Entity.hpp"

constexpr size_t MAX_COMPONENTS = 64;
using ComponentSignature = std::bitset<MAX_COMPONENTS>;
using ComponentType = std::uint16_t;

class IComponent
{
public:
    virtual ~IComponent() = default;
};

class ComponentRegistry {
private:
    // max of 2^16 component types
    static ComponentType _nextComponentType;
    static std::unordered_map<std::type_index, ComponentType> _componentTypes;
    static std::unordered_map<std::type_index, std::string> _componentNames;

public:
    template<typename TComponent>
    static ComponentType GetComponentType() {
        // do not allow to check random objects, just IComponents
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from Component!");

        // returns a unique type index
        auto type = std::type_index(typeid(TComponent));
        if (_componentTypes.find(type) == _componentTypes.end()) {
            _componentTypes[type] = _nextComponentType++;
            _componentNames[type] = typeid(TComponent).name();
        }
        return _componentTypes[type];
    }

    static size_t GetComponentCount() {
        return _nextComponentType;
    }
};


class IComponentVector
{
public:
	virtual ~IComponentVector() = default;
};

template<typename TComponent>
class ComponentVector : public IComponentVector
{
	std::vector<TComponent> _components;
	std::unordered_map<Entity, size_t> _entityToIndex;

public:
    TComponent& GetComponent(Entity entity)
    {
        auto it = _entityToIndex.find(entity);
        if (it != _entityToIndex.end())
            return _components[it->second];

        throw std::runtime_error("Component not found for entity!");
    }

	void AddComponent(Entity entity, const TComponent& component)
	{
		_components.push_back(component);
		_entityToIndex[entity] = _components.size() - 1;

		std::cout << _entityToIndex.size() << " added in componentvector" << std::endl;
	}
	void RemoveComponent();

public:
	ComponentVector() {};
	~ComponentVector() {};
};
