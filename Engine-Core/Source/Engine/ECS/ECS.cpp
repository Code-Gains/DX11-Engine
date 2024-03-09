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
}

void ECS::Update(float deltaTime)
{
}

void ECS::PeriodicUpdate(float deltaTime)
{
}
