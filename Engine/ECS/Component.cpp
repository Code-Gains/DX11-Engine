#include "Component.hpp"

//ComponentSet::ComponentSet()
//{
//}
//
//ComponentSet::ComponentSet(const std::vector<int> componentIds) : _componentIds(componentIds)
//{
//}
//
//int ComponentSet::GetId() const
//{
//	return _id;
//}
//
//std::vector<int> ComponentSet::getComponentIds() const
//{
//	return _componentIds;
//}

Component::Component()
{
}

Component::Component(Entity entity) : _entityIds(std::vector<int> {entity.GetId()})
{
}

Component::Component(int entityId) : _entityIds(std::vector<int> {entityId})
{
}

Component::Component(const std::vector<int>& _entityIds) : _entityIds(_entityIds)
{
}

void Component::AddEntity(int entityId)
{
	_entityIds.push_back(entityId);
}

void Component::AddEntity(Entity entity)
{
	_entityIds.push_back(entity.GetId());
}
