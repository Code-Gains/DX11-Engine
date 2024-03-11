#pragma once
#include <iostream>
#include <typeindex>
#include <unordered_map>

#include "Entity.hpp"

constexpr size_t MAX_COMPONENTS = 64;
using ComponentType = std::uint16_t;

class IComponent
{
public:
    virtual ~IComponent() = default;
};

class ComponentRegistry {
private:
    static ComponentType _nextComponentType;
    static std::unordered_map<std::type_index, ComponentType> _componentTypes;
    static std::unordered_map<std::type_index, std::string> _componentNames;

public:
    template<typename TComponent>
    static ComponentType GetComponentType() {
        static_assert(std::is_base_of<IComponent, TComponent>::value, "TComponent must inherit from Component");

        auto type = std::type_index(typeid(TComponent));
        if (_componentTypes.find(type) == _componentTypes.end()) {
            _componentTypes[type] = _nextComponentType++;
            _componentNames[type] = typeid(TComponent).name(); // Optional, for debugging or introspection
        }
        return _componentTypes[type];
    }

    static size_t getComponentCount() {
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

template<typename TComponent>
std::type_index GetComponentTypeId()
{
	return std::type_index(typeid(TComponent));
}