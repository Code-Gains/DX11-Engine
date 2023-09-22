#include "World.hpp"

Entity World::CreateEntity(const std::vector<int>& componentSet)
{
	int entityId = _nextId;
	auto entity = Entity(entityId);
	_entities[entityId] = ComponentStorageShelf(ComponentSet(componentSet));

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

	int entityComponentSetId = entityIterator->first;
	return _uniqueComponentSets.count(entityComponentSetId) != 0;
}

void World::UpdateEntities(float deltaTime)
{
}

size_t World::GetEntityCount() const
{
	return _entities.size();
}

ComponentStorage::ComponentStorage()
{
}

ComponentStorage::ComponentStorage(size_t componentSize, size_t componentCount) : _componentSize(componentSize), _componentCount(componentCount)
{
}

std::shared_ptr<void> ComponentStorage::GetElements() const
{
	return _elements;
}

size_t ComponentStorage::GetComponentSize() const
{
	return _componentSize;
}

size_t ComponentStorage::GetComponentCount() const
{
	return _componentCount;
}
