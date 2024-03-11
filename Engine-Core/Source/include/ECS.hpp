#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "Archetype.hpp"
#include "System.hpp"
#include "Entity.hpp"

class ECS
{
	std::vector<std::unique_ptr<ISystem>> _systems;
	std::unordered_map<ComponentSignature, Archetype> _signatureToArchetype;
	std::unordered_map<Entity, ComponentSignature> _entityToSignature;

	// ids
	std::uint32_t _nextEntityId = 0;

public:
	// Constructors
	ECS() {};
	~ECS() {};

	// Core Loops
	// Add loops here

	// Archetype Management
	Archetype& GetEntityArchetype(Entity entity) const;

	// Entity Management
	Entity GetNextEntityId() const;

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	// Component Management
	template<typename TComponent>
	void AddComponent(Entity entity, const TComponent& component) const
	{
		std::cout << "added in addedinecs" << std::endl;
		// determine archetype wit new component
		Archetype& archetype = GetEntityArchetype(entity);

		//auto typeId = GetComponentTypeId<TComponent>();
		//if()
		// find existing? if no then create new
		// move entity to the archetype container
		// add component to the entity in that archetype
	}

	template<typename TComponent>
	void RemoveComponent(Entity entity) const
	{

	}

	// Systems

	template<typename TSystem, typename... Args>
	TSystem& AddSystem(Args&&... args)
	{
		TSystem* system = new TSystem(std::forward<Args>(args)...);
		_systems.emplace_back(system);
		return *system;
	}
	
	// Loops
	void Render();
	void Update(float deltaTime);
	void PeriodicUpdate(float deltaTime);
};
