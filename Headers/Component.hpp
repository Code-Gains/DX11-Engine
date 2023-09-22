#pragma once
#include <vector>
class Component
{
	int _id;
	int _componentSetId;
public:
	int GetId() const;
};

/*
* Component set specifies which components it has.
*/
class ComponentSet
{
	int _id;
	std::vector<int> _componentIds;
public:
	ComponentSet();
	ComponentSet(const std::vector<int> componentIds);

	int GetId() const;
	std::vector<int> getComponentIds() const;
};
