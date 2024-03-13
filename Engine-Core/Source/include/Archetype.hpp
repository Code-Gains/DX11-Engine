#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <bitset>

#include "Entity.hpp"
#include "Component.hpp"

class Archetype
{
	//ComponentVector Container class - implement interfaces
	// bitmask?
	ComponentSignature _signature;
	std::vector<Entity> _entities;
	std::unordered_map<ComponentType, std::unique_ptr<IComponentVector>> _typeToComponentVector;

public:
	Archetype() {};
	Archetype(const ComponentSignature& signature);
	~Archetype() {};
	
	//template<typename TComponent>
	//TComponent* GetComponent(Entity entity, ComponentType componentType)
	//{
	//	auto it = _typeToComponent.find(componentType);
	//	if (it != _typeToComponent.end())
	//	{
	//		// Cast from interface to actual vector type (type safe)
	//		auto componentVector = dynamic_cast<ComponentVector<TComponent>*>(it->second.get());
	//		if (componentVector)
	//			return componentVector->GetComponent(entity);
	//	}

	//	//todo throw
	//	return nullptr;
	//}

	IComponentVector* GetComponentVector(ComponentType componentType);

	IComponentVector* GetOrCreateComponentVector(ComponentType componentType)
	{
		auto it = _typeToComponentVector.find(componentType);
		// vector exists
		if (it != _typeToComponentVector.end())
			return it->second.get();

		// vector does not exist
		auto factoryFunction = ComponentRegistry::GetFactoryFunction(componentType);
		auto newComponentVector = factoryFunction();
		_typeToComponentVector[componentType] = std::move(newComponentVector);

		return _typeToComponentVector[componentType].get();
	}

	ComponentSignature GetSignature() const;
	bool SignatureContainsBit(uint16_t bit) const;

	template<typename TComponent>
	void AddComponent(Entity entity, const TComponent& component, ComponentType componentType)
	{
		if (_typeToComponentVector.find(componentType) == _typeToComponentVector.end())
		{
			_typeToComponentVector[componentType] = std::make_unique<ComponentVector<TComponent>>();
			_signature.set(componentType, true);
		}

		// Retrieve component vector base and cast to component vector
		auto& vector = static_cast<ComponentVector<TComponent>&>(*_typeToComponentVector[componentType]);
		vector.AddComponent(entity, component);

		/*for (auto& typeToComponentVector : _typeToComponentVector)
		{
			auto componentVector = typeToComponentVector.second.get();
			std::cout << componentVector->GetComponentCount() << std::endl;
		}*/
	}

	//template<typename TComponent>
	//ComponentVector<TComponent>* GetComponentVector();
};