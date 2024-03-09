#include "ECS.hpp"

Entity ECS::GetNextEntityId() const
{
	return _nextEntityId;
}

Entity ECS::CreateEntity()
{
	return _nextEntityId++;
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
