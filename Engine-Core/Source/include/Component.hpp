#pragma once

class IComponentVector
{
public:
	virtual ~IComponentVector() = default;
};


template<typename TComponent>
class ComponentVector : public IComponentVector
{
	std::vector<TComponent> components;

	void AddComponent(TComponent component);
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