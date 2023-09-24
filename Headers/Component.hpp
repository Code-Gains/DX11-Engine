#pragma once
#include <vector>
#include "Entity.hpp"

class Component
{
	std::vector<int> _entityIds;
public:
	Component();
	Component(Entity entity);
	Component(int entityId);
	Component(const std::vector<int>& _entityIds);

	void AddEntity(int entityId);
	void AddEntity(Entity entity);
};

///*
//* Component set specifies which components it has.
//*/
//class ComponentSet
//{
//	int _id;
//	std::vector<int> _componentIds;
//public:
//	ComponentSet();
//	ComponentSet(const std::vector<int> componentIds);
//
//	int GetId() const;
//	std::vector<int> getComponentIds() const;
//};
