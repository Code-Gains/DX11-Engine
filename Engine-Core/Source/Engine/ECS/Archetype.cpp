#include "Archetype.hpp"
#include <stdlib.h>;

Archetype::Archetype(const ComponentSignature& signature) : _signature(signature)
{
}

void Archetype::DestroyEntity(Entity entity)
{
	// take each vector and remove entity data from it
	for (auto& typeToComponentVector : _typeToComponentVector)
	{
		// clears component data linked to an entity (swap + pop removal)
		typeToComponentVector.second->RemoveComponent(entity);
	}

}

IComponentVector* Archetype::GetComponentVector(ComponentType componentType) const
{
	auto it = _typeToComponentVector.find(componentType);
	if (it != _typeToComponentVector.end())
		return it->second.get();

	return nullptr;
}

ComponentSignature Archetype::GetSignature() const
{
	return _signature;
}

bool Archetype::SignatureContainsBit(uint16_t bit) const
{
	return _signature.test(bit);
}
