#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <bitset>

#include "Entity.hpp"
#include "Component.hpp"

class Archetype
{
	//ComponentVector Container class - implement interfaces
	// bitmask?
	ComponentSignature _signature;
	std::vector<Entity> _entities;
	std::unordered_map<ComponentType, std::unique_ptr<IComponentVector>> _typeToComponent;
public:

	Archetype() {};
	Archetype(const ComponentSignature& signature);
	~Archetype() {};

	//IComponent& GetComponent(Entity entity, ComponentType componentType);

	ComponentSignature GetSignature() const;
	bool SignatureContainsBit(uint16_t bit) const;

	template<typename TComponent>
	void AddComponent(Entity entity, const TComponent& component, ComponentType componentType)
	{
		if (_typeToComponent.find(componentType) == _typeToComponent.end())
		{
			_typeToComponent[componentType] = std::make_unique<ComponentVector<TComponent>>();
			_signature.set(componentType, true);
		}

		// Retrieve component vector base and cast to component vector
		auto& vector = static_cast<ComponentVector<TComponent>&>(*_typeToComponent[componentType]);
		vector.AddComponent(entity, component);
	}

	//template<typename TComponent>
	//ComponentVector<TComponent>* GetComponentVector();
};