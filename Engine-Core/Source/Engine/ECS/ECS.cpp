#include "ECS.hpp"

size_t ECS::GetArchetypeCount() const
{
	return _signatureToArchetype.size();
}

ComponentSignature ECS::GenerateNewSignature(std::optional<ComponentSignature> current, ComponentType componentType) const
{
	if (!current)
		return ComponentSignature().set(componentType);
	else
		return current->set(componentType);
}

ComponentSignature ECS::GenerateNewSignature(ComponentSignature current, ComponentType componentType) const
{
	return current.set(componentType);
}

Archetype* ECS::GetEntityArchetype(Entity entity) const
{
	auto signature = GetEntitySignature(entity);
	if (!signature)
		return nullptr;

	return GetSignatureArchetype(signature.value());
}

Archetype* ECS::GetSignatureArchetype(const ComponentSignature& signature) const
{
	auto it = _signatureToArchetype.find(signature);
	if (it != _signatureToArchetype.end())
		return it->second.get();

	return nullptr;
}

Archetype* ECS::GetOrCreateArchetype(const ComponentSignature& signature)
{
	auto existingArchetype = GetSignatureArchetype(signature);
	if (!existingArchetype)
	{
		auto newArchetype = std::make_unique<Archetype>(signature);
		_signatureToArchetype[signature] = std::move(newArchetype);
		return _signatureToArchetype[signature].get();
	}
	return existingArchetype;
}

const std::unordered_map<ComponentSignature, std::unique_ptr<Archetype>>& ECS::GetSignatureToArchetype() const
{
	return _signatureToArchetype;
}

const std::optional<ComponentSignature> ECS::GetEntitySignature(Entity entity) const
{
	auto it = _entityToSignature.find(entity);
	if (it != _entityToSignature.end())
		return it->second;

	return std::nullopt;
}

size_t ECS::GetEntityCount() const
{
	return _entityToSignature.size();
}

std::unordered_map<Entity, ComponentSignature> ECS::GetAllEntities() const
{
	return _entityToSignature;
}

Entity ECS::GetNextEntityId() const
{
	return _nextEntityId;
}

Entity ECS::CreateEntity()
{
	return _nextEntityId++;
}

void ECS::DestroyEntity(Entity entity)
{
	// clear component data
	auto archetype = GetEntityArchetype(entity);
	archetype->DestroyEntity(entity);

	// clear mapping
	auto it = _entityToSignature.find(entity);
	if (it != _entityToSignature.end())
		_entityToSignature.erase(it);
}

void ECS::Render()
{
	for (auto& system : _systems)
	{
		system->Render();
	}
}

void ECS::Update(float deltaTime)
{
	for (auto& system : _systems)
	{
		system->Update(deltaTime);
	}
}

void ECS::PeriodicUpdate(float deltaTime)
{
	for (auto& system : _systems)
	{
		system->PeriodicUpdate(deltaTime);
	}
}
