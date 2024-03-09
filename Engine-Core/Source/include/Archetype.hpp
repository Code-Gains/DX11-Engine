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

	void AddComponentType();

	template<typename TComponent>
	ComponentVector<TComponent>* GetComponentVector();
};