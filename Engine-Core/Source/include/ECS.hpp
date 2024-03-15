#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include "Archetype.hpp"
#include "System.hpp"
#include "Entity.hpp"

class ECS
{
	std::vector<std::unique_ptr<ISystem>> _systems;
	std::unordered_map<ComponentSignature, std::unique_ptr<Archetype>> _signatureToArchetype;
	std::unordered_map<Entity, ComponentSignature> _entityToSignature;

	// --- IDs ---
	std::uint32_t _nextEntityId = 0;

public:
	// --- Constructors ---
	ECS() {};
	~ECS() {};

	// --- Core Loops ---
	void Render();
	void Update(float deltaTime);
	void PeriodicUpdate(float deltaTime);

	// --- Archetype Management ---
	Archetype* GetEntityArchetype(Entity entity) const;
	Archetype* GetSignatureArchetype(const ComponentSignature& signature) const;

	// --- Entity Management ---
	Entity GetNextEntityId() const;
	const std::optional<ComponentSignature> GetEntitySignature(Entity entity) const;

	void TransferEntityComponents(Entity entity, Archetype* from, Archetype* to)
	{
		for (ComponentType ct = 0; ct < MAX_COMPONENTS; ++ct)
		{
			if (from->GetSignature().test(ct))
			{
				auto fromVector = from->GetComponentVector(ct);
				auto toVector = to->GetOrCreateComponentVector(ct);

				if (fromVector && toVector)
					fromVector->TransferComponent(entity, *toVector);
				else
					throw std::runtime_error("Could not get or create component vectors!");

			}
		}
	}

	Entity CreateEntity();
	//void DestroyEntity(Entity entity);

	// --- Component Management ---
	template<typename TComponent>
	void AddComponent(Entity entity, const TComponent& component)
	{
		std::cout << "Component has been added!\n";
		// perform inheritance checks and get type
		auto componentType = ComponentRegistry::GetOrRegisterComponentType<TComponent>();

		// check if entity already belongs to an archetype
		auto signature = GetEntitySignature(entity);
		if (!signature) // single component archetype
		{
			//std::cout << "added new in addedinecs" << std::endl;

		    auto newArchetype = std::make_unique<Archetype>();
			newArchetype->AddComponent<TComponent>(entity, component, componentType);

			auto newSignature = newArchetype->GetSignature();
			_signatureToArchetype[newSignature] = std::move(newArchetype);
			_entityToSignature[entity] = newSignature;
			return;
		}
		// multiple component archetype
		auto previousArchetype = GetSignatureArchetype(signature.value());

		// check if the component we are adding currently exists in the archetype
		if (previousArchetype->SignatureContainsBit(componentType))
		{
			previousArchetype->AddComponent<TComponent>(entity, component, componentType);
			return;
		}

		// we are adding a new type of component, we will have to first remove the data from previous archetype,
		// then create a new archetype based on the previous one
		auto newArchetype = std::make_unique<Archetype>(previousArchetype->GetSignature());
		TransferEntityComponents(entity, previousArchetype, newArchetype.get());
		newArchetype->AddComponent<TComponent>(entity, component, componentType);
		auto newSignature = newArchetype->GetSignature();
		_signatureToArchetype[newSignature] = std::move(newArchetype);
		_entityToSignature[entity] = newSignature;
	}

	template<typename TComponent>
	void RemoveComponent(Entity entity) const
	{

	}

	// --- Systems ---

	template<typename TSystem, typename... Args>
	TSystem& AddSystem(Args&&... args)
	{
		TSystem* system = new TSystem(std::forward<Args>(args)...);
		_systems.emplace_back(system);
		return *system;
	}
	 
	// --- Querying ---

	// template is a list of component types <CT1, CT2, ... , CT(N)> (c++11 variadic templates)
	template<typename... TComponents>
	std::vector<std::tuple<std::vector<TComponents>*...>> QueryComponentVectors()
	{
		std::vector<std::tuple<std::vector<TComponents>*...>> results;

		// pass the component list to component registry to try and generate a signature (c++17 fold expression)
		auto querySignatureOpt = ComponentRegistry::GetComponentArraySignature<TComponents...>();

		// early exit if signature has not been generated
		if (!querySignatureOpt.has_value())
			return results;

		auto querySignature = querySignatureOpt.value();

		// iterate through all archetypes and find the ones that include all components in signature
		for (auto& [signature, archetype] : _signatureToArchetype)
		{
			// compare matching bits to the query
			if ((signature & querySignature) == querySignature)
			{
				auto componentVectorsTuple = std::make_tuple(archetype->GetComponentVector<TComponents>()...);
				// check if all vectors are present from the archetype might not need in release config


				auto areAllVectorsNonNull = [](auto... vectors) -> bool
				{
					return (vectors && ...);
				};

				if (areAllVectorsNonNull(std::get<std::vector<TComponents>*>(componentVectorsTuple) ...))
					results.push_back(componentVectorsTuple);
			}
		}

		return results;
	}

	/*void GetEntitiesWithSignature(const ComponentSignature& querySignature)
	{

	}*/
};
