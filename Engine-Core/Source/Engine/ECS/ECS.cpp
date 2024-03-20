#include "ECS.hpp"

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

	std::cerr << "Signature does not have an archetype!";
	return nullptr;
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

Entity ECS::GetNextEntityId() const
{
	return _nextEntityId;
}

Entity ECS::CreateEntity()
{
	//std::cout << _nextEntityId << std::endl;
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
