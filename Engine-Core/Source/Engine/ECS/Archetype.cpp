#include "Archetype.hpp"

Archetype::Archetype(const ComponentSignature& signature) : _signature(signature)
{
}

//IComponent Archetype::GetComponent(Entity entity, ComponentType componentType)
//{
//	auto it = _typeToComponent.find(componentType);
//	if(it != _typeToComponent.end())
//		return it->second->
//}

ComponentSignature Archetype::GetSignature() const
{
	return _signature;
}

bool Archetype::SignatureContainsBit(uint16_t bit) const
{
	return _signature.test(bit);
}
