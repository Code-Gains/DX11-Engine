#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "Entity.hpp"
#include "Component.hpp"

#include "TransformComponent.hpp"
#include "MeshComponent.hpp"
#include "MaterialComponent.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"

/*
* Since it is not possible to determine what types would be contained in vectors at compile time
* we have to manage the data ourselves, essentially risking accessing memory out of bounds :)
*/
class ComponentStorage
{
	// Store data without determining the type at compile time
	// once needed perform a static cast to allow access for methods
	// there is info that static casts might be optimized entirely by the compiler
	std::shared_ptr<void> _elements;
	size_t _componentSize;
	size_t _componentCount;

public:
	ComponentStorage();
	ComponentStorage(size_t componentSize, size_t componentCount);

	std::shared_ptr<void> GetElements() const;
	size_t GetComponentSize() const;
	size_t GetComponentCount() const;
};


class ComponentStorageShelf
{
	ComponentSet _componentSet;
	size_t _shelf;
public:
	ComponentStorageShelf() {};
	ComponentStorageShelf(ComponentSet componentSet) : _componentSet(componentSet) {};
};

/*
* Should be a singleton class responsible for all entity/component management?
*/
class World
{
	// Relations
	std::unordered_map<int, ComponentStorageShelf> _entities; // entities are sets of components
	std::unordered_map<int, std::vector<int>> _components; // what sets components belong to
	std::unordered_set<int> _uniqueComponentSets;

	//Storage

	// we will want to use a structure that is continuous in memory and can be sorted easily
	/*template<typename ComponentType>
	std::unordered_map<int, std::vector<ComponentType>> _components;*/
	// Can't determine what will be inside of the vectors so can't compile

	// This is going to be a nightmare i can feel it
	//std::unordered_map<int, ComponentStorage> _componentData; 
	std::vector<ComponentSet> _componentSetData; // all current component set variations

	// Indexing
	int _nextId = 1;
public:
	Entity CreateEntity(const std::vector<int>& componentSet);
	int DeleteEntity(int id);
	bool HasEntityComponent(int entityId, int componentId) const;
	void UpdateEntities(float deltaTime);

	size_t GetEntityCount() const;
};

