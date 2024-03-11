#pragma once
#include <iostream>

class IComponentVector
{
public:
	virtual ~IComponentVector() = default;
};


template<typename TComponent>
class ComponentVector : public IComponentVector
{
	std::vector<TComponent> _components;
	std::unordered_map<Entity, size_t> _entityToIndex;

public:
	template<typename TComponent>
	void AddComponent(Entity entity, const TComponent& component)
	{
		_components.push_back(component);
		_entityToIndex[entity] = _components.size() - 1;

		std::cout << _entityToIndex.size() << " added in componentvector" << std::endl;
	}
	void RemoveComponent();

public:
	ComponentVector() {};
	~ComponentVector() {};
};

template<typename TComponent>
std::type_index GetComponentTypeId()
{
	return std::type_index(typeid(TComponent));
}