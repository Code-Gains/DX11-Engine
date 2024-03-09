#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <bitset>

#include "Entity.hpp"
#include "Component.hpp"

constexpr size_t MAX_COMPONENTS = 64;
using ComponentType = std::uint16_t;
using ComponentSignature = std::bitset<MAX_COMPONENTS>;

class Archetype
{
	//ComponentVector Container class - implement interfaces
	// bitmask?
	ComponentSignature _signature;
	std::vector<Entity> _entities;
	std::unordered_map<std::type_index, std::unique_ptr<IComponentVector>> _componentVectors;
public:

	Archetype() {};
	~Archetype() {};

	template<typename TComponent>
	void AddComponent(Entity entity, TComponent component)
	{
		auto typeId = GetComponentTypeId<TComponent>();
		if (_componentVectors.find(typeId) == _componentVectors.end())
		{
			_componentVectors[typeId] = std::make_unique<ComponentVector<TComponent>>();
			// create a new vector in the archetype
		}
		// Retrieve component vector base and cast to component vector
		auto& vector = static_cast<ComponentVector<TComponent>&>(*_componentVectors[typeId]);
		


		// find existing? if no then create new
		// move entity to the archetype container
		// add component to the entity in that archetype
	}

	template<typename TComponent>
	ComponentVector<TComponent>* GetComponentVector();
};