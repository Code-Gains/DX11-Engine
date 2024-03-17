#include "Archetype.hpp"
#include <stdlib.h>;

Archetype::Archetype(const ComponentSignature& signature) : _signature(signature)
{
}

IComponentVector* Archetype::GetComponentVector(ComponentType componentType)
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
