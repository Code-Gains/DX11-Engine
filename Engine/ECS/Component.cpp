#include "Component.hpp"

int Component::GetId() const
{
	return _id;
}

ComponentSet::ComponentSet()
{
}

ComponentSet::ComponentSet(const std::vector<int> componentIds) : _componentIds(componentIds)
{
}

int ComponentSet::GetId() const
{
	return _id;
}

std::vector<int> ComponentSet::getComponentIds() const
{
	return _componentIds;
}
