#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <bitset>
#include <any>

#include "Entity.hpp"
#include "Component.hpp"
#include <stdlib.h>;

class Archetype
{
	ComponentSignature _signature;
	//std::vector<Entity> _entities;
	std::unordered_map<ComponentType, std::unique_ptr<IComponentVector>> _typeToComponentVector;

public:
	Archetype() {};
	Archetype(const ComponentSignature& signature);
	~Archetype() {};

	// Entity management
	void DestroyEntity(Entity entity);

	// Components

	template<typename TComponent>
	TComponent* GetComponent(Entity entity, ComponentType componentType) const
	{
		auto it = _typeToComponentVector.find(componentType);
		if (it != _typeToComponentVector.end())
		{
			void* component = it->second.get()->GetComponent(entity);
			return static_cast<TComponent*>(component);
		}

		return nullptr;
	}

	// Component Vectors

	const std::unordered_map<ComponentType, std::unique_ptr<IComponentVector>>& GetTypeToComponentVector() const;
	IComponentVector* GetComponentVector(ComponentType componentType) const;

	template <typename TComponent>
	IComponentVector* GetComponentVector()
	{
		auto componentTypeOpt = ComponentRegistry::GetComponentType<TComponent>();
		if (!componentTypeOpt)
			return nullptr;

		auto it = _typeToComponentVector.find(componentTypeOpt.value());
		if (it != _typeToComponentVector.end())
			return it->second.get();

		return nullptr;
	}

	template <typename TComponent>
	ComponentVector<TComponent>* GetComponentVectorCast()
	{
		auto componentTypeOpt = ComponentRegistry::GetComponentType<TComponent>();
		if (!componentTypeOpt)
			return nullptr;

		auto it = _typeToComponentVector.find(componentTypeOpt.value());
		if (it != _typeToComponentVector.end())
			return static_cast<ComponentVector<TComponent>*>(it->second.get());

		return nullptr;
	}

	template <typename TComponent>
	std::vector<TComponent>* GetComponentVectorCastRaw()
	{
		auto componentTypeOpt = ComponentRegistry::GetComponentType<TComponent>();
		if (!componentTypeOpt)
			return nullptr;

		auto it = _typeToComponentVector.find(componentTypeOpt.value());
		if (it != _typeToComponentVector.end())
			return static_cast<std::vector<TComponent>*>(it->second.get().GetRawVector());

		return nullptr;
	}

	IComponentVector* GetOrCreateComponentVector(ComponentType componentType)
	{
		auto it = _typeToComponentVector.find(componentType);
		// vector exists
		if (it != _typeToComponentVector.end())
			return it->second.get();

		// vector does not exist
		auto factoryFunction = ComponentRegistry::GetFactoryFunction(componentType);
		auto newComponentVector = factoryFunction();
		_typeToComponentVector[componentType] = std::move(newComponentVector);

		return _typeToComponentVector[componentType].get();
	}

	ComponentSignature GetSignature() const;
	bool SignatureContainsBit(uint16_t bit) const;

	template<typename TComponent>
	void AddComponent(Entity entity, const TComponent& component, ComponentType componentType)
	{
		if (_typeToComponentVector.find(componentType) == _typeToComponentVector.end())
		{
			_typeToComponentVector[componentType] = std::make_unique<ComponentVector<TComponent>>();
			_signature.set(componentType, true);
		}

		// Retrieve component vector base and cast to component vector
		auto& vector = static_cast<ComponentVector<TComponent>&>(*_typeToComponentVector[componentType]);
		vector.AddComponent(entity, component);
	}
};