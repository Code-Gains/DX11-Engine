#include "ComponentIdentifier.hpp"

ComponentIdentifier::ComponentIdentifier(int id) : _id(id)
{
}

void ComponentIdentifier::SetId(int id)
{
	_id = id;
}

int ComponentIdentifier::GetId() const
{
	return _id;
}
