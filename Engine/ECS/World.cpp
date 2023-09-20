#include "World.hpp"

Entity World::CreateEntity(const ComponentSet& componentSet)
{
	int entityId = _nextId;
	auto entity = Entity(entityId);
	_entities[entityId] = componentSet;

	_nextId++;
	return entity;
}

int World::DeleteEntity(int id)
{
	_entities.erase(id);
	return id;
}

bool World::HasEntityComponent(int entityId, int componentId) const
{
	auto entityIterator = _entities.find(entityId);

	if (entityIterator == _entities.end())
		return false;

	auto& entityComponentSet = entityIterator->second;
	return _uniqueComponentSets.count(entityComponentSet.GetId()) != 0;
}
