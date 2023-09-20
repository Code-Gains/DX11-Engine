#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Entity.hpp"
#include "Component.hpp"

/*
* Should be a singleton class responsible for all entity/component management?
*/
class World
{
	std::unordered_map<int, ComponentSet> _entities;
	std::unordered_set<int> _uniqueComponentSets;
	int _nextId = 1;
public:
	Entity CreateEntity(const ComponentSet& componentSet);
	int DeleteEntity(int id);
	
	bool HasEntityComponent(int entityId, int componentId) const;
};

